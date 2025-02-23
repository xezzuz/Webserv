#include "CGIHandler.hpp"

void	CGIHandler::validateHeaders()
{
	std::map<std::string, std::string>::iterator field;

	// Content-Type
	field = headersMap.find("content-type");
	if (field == headersMap.end() && !buffer.empty())
	{
		std::cerr << "[WEBSERV][ERROR]\tContet-Type Header Was Not Found In CGI" << std::endl;
		throw(500);
	}

	// ContentLength
	field = headersMap.find("content-length");
	if (field == headersMap.end())
	{
		headers.append("\r\nTransfer-Encoding: chunked");
		CGIreader = &CGIHandler::readCGIChunked;
		if (!buffer.empty())
			buffer = buildChunk(buffer.c_str(), buffer.size());
	}

	// Location
	std::string location;
	field = headersMap.find("location");
	if (headersMap.find("location") != headersMap.end())
	{
		location = field->second;
		if (location.at(0) == '/')
			throw(CGIRedirectException(location));
	}

	// Status
	field = headersMap.find("status");
	if (headersMap.find("status") != headersMap.end())
	{
		std::string statusStr = field->second;
		headersMap.erase(field);

		size_t numStart = statusStr.find_first_not_of(" \t\f\v");
		size_t numEnd = statusStr.find_first_of(" \t\f\v", numStart);
		std::string numStr = statusStr.substr(numStart, numEnd - numStart);

		char *stop;
		int statusCode = strtoul(numStr.c_str(), &stop, 10);
		if (errno == ERANGE || *stop)
		{
			std::cerr << "[WEBSERV][ERROR]\tMalformed Status Header In CGI" << std::endl;
			throw(500);
		}
		if (statusCode == 302 && location.empty()) // RFC 3875 section 6.3.3
		{
			std::cerr << "[WEBSERV][ERROR]\t302 Status Code With No Location Header" << std::endl;
			throw(500);
		}
		headers.insert(0, "HTTP/1.1 " + _toString(statusCode) + " " + stringtrim(statusStr.substr(numEnd), " \t\f\v"));
	}
	else
		headers.insert(0, "HTTP/1.1 " + _toString(reqCtx->StatusCode) + " " + statusCodes[reqCtx->StatusCode]);
}

void	CGIHandler::parseHeaders()
{
	size_t CRLF = buffer.find("\r\n\r\n");
	if (CRLF == std::string::npos)
	{
		std::cerr << "[WEBSERV][ERROR]\tCGI Headers Not Found" << std::endl;
		throw(500);
	}
	std::stringstream headerStream(buffer.substr(0, CRLF + 2));
	buffer.erase(0, CRLF + 4);

	std::string field;

	while (getline(headerStream, field, '\n'))
	{
		if (field.empty())
			break;
		
		if (field[field.size() - 1] == '\r')
			field.erase(field.size() - 1);
	
	
		std::cout << "Field :::::::" << field << "::::::: End_Field" << std::endl;
		size_t pos = field.find(':');
		if (pos == std::string::npos)
		{
			std::cerr << "[WEBSERV][ERROR]\tMalformed CGI Headers" << std::endl;
			throw(500);
		}
		std::string key = stringtolower(field.substr(0, pos));
		std::string value = stringtrim(field.substr(pos + 1), " \t\v\f");
		if (value.empty())
			continue ; // RFC 3875 section 6.3
		if (key.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-") != std::string::npos)
		{
			std::cerr << "[WEBSERV][ERROR]\tMalformed Headers Field-Name" << std::endl;
			throw(500);
		}
		headersMap[key] = value;
	}
}

void	CGIHandler::addHeaders()
{
	std::map<std::string, std::string>::iterator field;

	for (field = headersMap.begin(); field != headersMap.end(); field++)
		headers.append("\r\n" + capitalize(field->first) + ": " + field->second);
}

void	CGIHandler::generateHeaders()
{
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");

	parseHeaders();
	validateHeaders();
	addHeaders();
	headers.append("\r\n\r\n");

	std::cout << headers;
	headersParsed = true;
	if ((this->*sender)() == true && buffer.empty()) //
		state = nextState;
	else
		state = WRITE;
	// std::cout << "_______" << std::endl;
	// std::cout << headers ;
	// std::cout << "_______" << std::endl;
}