/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:26:22 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/27 14:19:43 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

bool			Request::decodeURI() {
	std::string			encodedURI = _RequestData.URI;
	std::string			decodedURI;

	for (size_t i = 0; i < encodedURI.size(); i++) {
		if (encodedURI[i] == '%' && i + 2 < encodedURI.size() && isHexa(encodedURI.substr(i + 1, 2))) {
			decodedURI += htoi(encodedURI.substr(i + 1, 2));
			i += 2;
		} else if (encodedURI[i] == '+') {
			decodedURI += ' ';
		} else {
			decodedURI += encodedURI[i];
		}
	}
	_RequestData.URI = decodedURI;
	return true;
}

void	Request::isValidMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw (501);

	if (std::find(_RequestData._Config->methods.begin(), _RequestData._Config->methods.end(), method) == _RequestData._Config->methods.end())
		throw(405);
}

void	Request::isValidURI(const std::string& uri) 
{
	if (uri.size() > 2048)
		throw(414);

	if (uri[0] != '/') {
		throw(400);
	}
	
	static char allowedURIChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._-~:/?#[]@!$&'()*+,;=%";
	
	if (uri.find_first_not_of(allowedURIChars) != std::string::npos)
		throw(400);
}

void	Request::isValidHTTPVersion(const std::string& httpversion)
{
	if (httpversion.length() != 8 || httpversion.substr(0, 5) != "HTTP/")
		throw(400);
	if (httpversion.substr(5) != "1.1")
		throw(505);
}

// this function should be revised against RFC
void	Request::validateRequestHeaders()
{
	std::map<std::string, std::string>::iterator header;
	header = _RequestData.Headers.find("host");
	if (header != _RequestData.Headers.end())
		_RequestData.host = header->second;
	
	if (_RequestData.host.empty())
		throw(400);

	header = _RequestData.Headers.find("connection");
	if (header != _RequestData.Headers.end()) {
		_RequestData.connection = header->second;
		_RequestData.keepAlive = header->second == "keep-alive";
	}
	else
		_RequestData.Headers.insert(std::make_pair("connection", "keep-alive"));
	
	if (_RequestData.Method == "POST") {

		header = _RequestData.Headers.find("transfer-encoding");
		if (header != _RequestData.Headers.end())
		{
			_RequestData.transferEncoding = stringtolower(header->second);
			if (_RequestData.transferEncoding != "chunked")
				throw(501);
			isEncoded = true;
		}
	
		header = _RequestData.Headers.find("content-length");
		if (header != _RequestData.Headers.end() && stringIsDigit(header->second))
		{
			if (!_RequestData.transferEncoding.empty())
				throw(400);
			char	*stop;
			_RequestData.contentLength = std::strtoul(header->second.c_str(), &stop, 10); // 10: base decimal
			if (errno == ERANGE || *stop)
				throw(400);
		}

		header = _RequestData.Headers.find("content-type");
		
		if (header != _RequestData.Headers.end())
		{
			size_t colPos = header->second.find(";");
			if (colPos != std::string::npos)
			{
				_RequestData.contentType = header->second.substr(0, colPos);
				isMultipart = (_RequestData.contentType == "multipart/form-data");
				
				size_t boundaryPos = _RequestData.contentType.find("boundary=", colPos);
				if (boundaryPos == std::string::npos)
					throw(400);

				_RequestRaws.boundaryBegin = "--" + _RequestData.contentType.substr(boundaryPos + 9);
				if (_RequestRaws.boundaryBegin.length() > 72) // RFC 2046, Section 5.1.1
					throw(400);
				_RequestRaws.boundaryEnd = _RequestRaws.boundaryBegin + "--";
			}
			else
				_RequestData.contentType = header->second;
		}
		else
			_RequestData.contentType = "application/octet-stream";
	}
}

void	Request::parseHeaders() {
	size_t				CRLF = buffer.find("\r\n\r\n");
	std::istringstream	ssHeaders(buffer.substr(0, CRLF + 4));
	buffer.erase(0, CRLF + 4);
	bufferSize -= (CRLF + 4);

	static const char 	allowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'*+-.^_`|~";
	std::string			line;

	while (std::getline(ssHeaders, line))
	{
		if (line == "\r")
			break;

		size_t col = line.find(":");
		if (col == std::string::npos)
			throw(400);

		std::string key = stringtolower(line.substr(0, col));
		if (key.find_first_not_of(allowedChars) != std::string::npos)
			throw(400);
		if (_RequestData.Headers.find(key) != _RequestData.Headers.end())
			throw(400);

		std::string value = stringtrim(line.substr(col + 1), " \r\n\t\v");

		_RequestData.Headers.insert(std::make_pair(key, value));
	}
}

void	Request::parseRequestLine() {
	size_t CRLF = buffer.find("\r\n");
	_RequestRaws.rawRequestLine = buffer.substr(0, CRLF);
	buffer.erase(0, CRLF + 2);
	bufferSize -= (CRLF + 2);
	
	if (std::count(_RequestRaws.rawRequestLine.begin(), _RequestRaws.rawRequestLine.end(), ' ') != 2)
		throw (400);

	std::istringstream	ssLine(_RequestRaws.rawRequestLine);
	
	std::getline(ssLine, _RequestData.Method, ' ');
	isValidMethod(_RequestData.Method);
	
	std::getline(ssLine, _RequestData.URI, ' ');
    isValidURI(_RequestData.URI);
	
	std::getline(ssLine, _RequestData.HTTPversion, '\r');
    isValidHTTPVersion(_RequestData.HTTPversion);

	decodeURI();
}
