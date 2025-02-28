#include "CGIHandler.hpp"

void	CGIHandler::validateHeaders()
{
	std::map<std::string, std::string>::iterator field;

	// Content-Type
	field = headersMap.find("content-type");
	if (field == headersMap.end() && !buffer.empty())
	{
		std::cerr << "[CGI][ERROR]\tHEADER[Contet-Type] NOT PROVIDED BY SCRIPT" << std::endl;
		throw(500);
	}

	// ContentLength
	field = headersMap.find("content-length");
	if (field == headersMap.end() && !buffer.empty())
	{
		headers.append("\r\nTransfer-Encoding: chunked");
		CGIreader = &CGIHandler::readCGIChunked;
		buffer = buildChunk(buffer.c_str(), buffer.size());
	}

	// Location
	std::string location;
	field = headersMap.find("location");
	if (headersMap.find("location") != headersMap.end())
	{
		location = field->second;
		if (location.at(0) == '/')
			throw(CGIRedirect(location));
	}

	// Status
	field = headersMap.find("status");
	if (headersMap.find("status") != headersMap.end())
	{
		std::vector<std::string> statusVec;
		split(field->second, " \t\f\v", statusVec);
		if (statusVec.size() != 2)
		{
			std::cerr << "[CGI][ERROR]\tMALFORMED STATUS HEADER IN CGI" << std::endl;
			throw(500);
		}

		char *stop;
		int statusCode = strtoul(statusVec[0].c_str(), &stop, 10);
		if (errno == ERANGE || *stop)
		{
			std::cerr << "[CGI][ERROR]\tMALFORMED STATUS HEADER IN CGI" << std::endl;
			throw(500);
		}

		if (statusCode == 302 && location.empty()) // RFC 3875 section 6.3.3
		{
			std::cerr << "[CGI][ERROR]\t302 STATUS CODE WITH NO LOCATION HEADER" << std::endl;
			throw(500);
		}
		headers.insert(0, "HTTP/1.1 " + _toString(statusCode) + " " + statusVec[1]);

		headersMap.erase(field);
	}
	else
		headers.insert(0, "HTTP/1.1 " + _toString(reqCtx->StatusCode) + " " + statusCodes[reqCtx->StatusCode]);
}

void	CGIHandler::parseHeaders()
{
	size_t CRLF = buffer.find("\r\n\r\n");
	if (CRLF == std::string::npos)
	{
		std::cerr << "[CGI][ERROR]\tCGI HEADERS NOT FOUND" << std::endl;
		throw(500);
	}
	size_t	start = 0;
	size_t	end = 0;


	while (start < CRLF)
	{
		end = buffer.find("\r\n", start);
		if (end == std::string::npos || end > CRLF)
			break;
	
		std::string field = buffer.substr(start, end - start);

		size_t pos = field.find(':');
		if (pos == std::string::npos)
		{
			std::cerr << "[CGI][ERROR]\tMALFORMED CGI HEADERS" << std::endl;
			throw(500);
		}

		std::string key = stringtolower(field.substr(0, pos));
		std::string value = stringtrim(field.substr(pos + 1), " \t\v\f");
		if (value.empty())
		{
			start = end + 2;
			continue ; // RFC 3875 section 6.3
		}

		for (size_t i = 0; i < key.size(); i++)
        {
            if (!(std::isalnum(key[i]) || key[i] == '-'))
            {
                std::cerr << "[CGI][ERROR]\tMALFORMED HEADER[" << key << "]" << std::endl;
                throw 500;
            }
        }
		headersMap[key] = value;
	
		start = end + 2;
	}

	buffer.erase(0, CRLF + 4);
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

	headersParsed = true;
	if ((this->*sender)() == true && buffer.empty()) //
		state = nextState;
	else
		state = WRITE;
}