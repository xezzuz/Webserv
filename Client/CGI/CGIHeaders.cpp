#include "CGIHandler.hpp"
#include "../Response/Error.hpp"

void	CGIHandler::processCGIHeaders(std::map<std::string, std::string>& headersMap)
{
	if (headersMap.find("content-type") != headersMap.end())
	{
		headers.append("\r\nContent-Type: " + headersMap["content-type"]);
		headersMap.erase("content-type");
	}
	else if (!buffer.empty()) // RFC 3875 section 6.3.1
	{
		std::cerr << "[WEBSERV][ERROR]\tContet-Type Header Was Not Found In CGI" << std::endl;
		throw(500);
	}

	if (headersMap.find("content-length") != headersMap.end())
	{
		headers.append("\r\nContent-Length: " + headersMap["content-length"]);
		headersMap.erase("content-length");
	}
	else
	{
		headers.append("\r\nTransfer-Encoding: chunked");
		chunked = true;
	}

	std::string location;
	if (headersMap.find("location") != headersMap.end())
	{
		location = headersMap["location"];
		if (location.at(0) == '/')
			throw(CGIRedirectException(location));
		headersMap.erase("location");
	}
	if (headersMap.find("status") != headersMap.end())
	{
		std::string statusStr = headersMap["status"];
		headersMap.erase("status");

		size_t numStart = statusStr.find_first_not_of(" \t\f\v");
		size_t numEnd = statusStr.find_first_of(" \t\f\v", numStart);
		char *end;
		std::string numStr = statusStr.substr(numStart, numEnd - numStart);
		int statusCode = strtoul(numStr.c_str(), &end, 10);
		if (errno == ERANGE || *end)
		{
			std::cerr << "[WEBSERV][ERROR]\tMalformed Status Header In CGI" << std::endl;
			throw(500);
		}
		statusStr.erase(0, numEnd);
		statusStr = stringtrim(statusStr, " \t\f\v");
		if (statusCode == 302 && location.empty()) // RFC 3875 section 6.3.3
		{
			std::cerr << "[WEBSERV][ERROR]\t302 Status Code With No Location Header" << std::endl;
			throw(500);
		}
		else
			headers.append("\r\nLocation: " + location);
		headers.insert(0, "HTTP/1.1 " + _toString(statusCode) + " " + statusStr);
	}
	else
		headers.insert(0, "HTTP/1.1 " + _toString(reqCtx->StatusCode) + " " + statusCodes[reqCtx->StatusCode]);
	
	for (std::map<std::string, std::string>::iterator itr = headersMap.begin(); itr != headersMap.end(); itr++)
	{
		std::string key = itr->first;
		capitalize(key);
		headers.append("\r\n" + key + ": " + itr->second);
	}

	headers.append("\r\n\r\n");
}

void	CGIHandler::parseCGIHeaders()
{
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		std::cerr << "[WEBSERV][ERROR]\tCGI Headers Not Found" << std::endl;
		throw(500);
	}

	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());
	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");
	
	std::stringstream headerStream(buffer.substr(0, pos + 2));
	buffer.erase(0, pos + 4);

	std::map<std::string, std::string>	headersMap;
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

	processCGIHeaders(headersMap);
	parseBool = false;
	// std::cout << "_______" << std::endl;
	// std::cout << headers ;
	// std::cout << "_______" << std::endl;
}