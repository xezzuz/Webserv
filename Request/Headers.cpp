/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:26:22 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/21 15:51:40 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"


bool			Request::headerExists(const std::string& key) {
	std::map<std::string, std::string>::iterator it;
	it = header.headersMap.find(key);
	if (it == header.headersMap.end())
		return false;
	return true;
}

bool			Request::bufferContainHeaders() {
	size_t			pos;
	
	pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;
	return true;
}

std::string		Request::extractHeadersFromBuffer() {
	size_t			CRLF;
	std::string		header;
	
	CRLF = buffer.find("\r\n\r\n");
	if (CRLF == std::string::npos)
		return "";
	header = buffer.substr(0, CRLF) + "\r\n\r\n";
	buffer = buffer.substr(CRLF + 4);
	bufferSize -= (CRLF + 4);
	// std::cout << "[REQUEST]\tRequest got Splitted! NewBufferSize = " << bufferSize << std::endl;
	// for (size_t i = 0; i < bufferSize; i++) {
	// 	std::cout << buffer[i];
	// }
	// std::cout << "|" << std::endl;
	return header;
}

bool			isValidMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		return false;
	return true;
}

bool			isValidURI(const std::string& uri) {
	if (uri.size() > 2048) {
		// URI too long
		return false;
	}

	std::string	specialChars = "-._~:/?#[]@!$&'()*+,;=%";
	for (size_t i = 0; i < uri.size(); i++) {
		if (specialChars.find(uri[i]) == std::string::npos
			&& isdigit(uri[i]) && !isalpha(uri[i])) {
			return false;
		}
	}
	return true;
}

bool			isValidHTTPVersion(const std::string& httpversion) {
	if (httpversion != "HTTP/1.0" && httpversion != "HTTP/1.1")
		return false;
	return true;
}

bool			Request::isValidFieldLine(const std::string& fieldline) {
	std::string			line(fieldline);
	size_t				colonPos;

	colonPos = line.find(':');
	if (colonPos == std::string::npos)
		return false;
	
	std::string			fieldName = line.substr(0, colonPos);
	if (fieldName.empty() || headerExists(fieldName))
		return false;
	
	// check for fieldName invalid characters?

	std::string			fieldValue = line.substr(colonPos + 1);
	// if (fieldValue.empty())
	// 	return false;
	// if (fieldValue[0] == ' ' || fieldValue[0] == '\t')
	// 	fieldValue.erase(0, 1);
	
	// check for fieldValue invalid characters?
	return true;
}

bool			Request::parseHeaders() {
	std::string			fieldname;
	std::string			fieldvalue;

	for (size_t i = 1; i < header.rawHeaders.size(); i++) {
		if (header.rawHeaders[i].empty())
			continue;
		if (!isValidFieldLine(header.rawHeaders[i]))
			return false;
		int colonPos = header.rawHeaders[i].find(':');
		fieldname = header.rawHeaders[i].substr(0, colonPos);
		fieldvalue = stringtrim(header.rawHeaders[i].substr(colonPos + 1), " \t");
		header.headersMap[fieldname] = fieldvalue;
	}

	std::map<std::string, std::string>::iterator it;
	it = header.headersMap.find("Host");
	if (it != header.headersMap.end())
		header.host = it->second;
	
	it = header.headersMap.find("Content-Type");
	if (it != header.headersMap.end())
		header.contentType = it->second;
	
	it = header.headersMap.find("Connection");
	if (it != header.headersMap.end())
		header.connection = it->second;
	
	it = header.headersMap.find("Transfer-Encoding");
	if (it != header.headersMap.end())
		header.transferEncoding = stringtolower(it->second);
	
	it = header.headersMap.find("Content-Length");
	if (it != header.headersMap.end() && stringIsDigit(it->second))
		body.contentLength = std::atoi(it->second.c_str());
	
	return true;
}

bool			Request::parseRequestLine() {
	std::string			token;
	std::string			tokens[3];
	size_t				tokenCount = 0;
	size_t				spaceCount = 0;
	std::stringstream	ss(header.rawHeaders[0]);

	requestLine.rawRequestLine = header.rawHeaders[0];
	for (size_t i = 0; requestLine.rawRequestLine[i]; i++)
		if (requestLine.rawRequestLine[i] == ' ')
			spaceCount++;
	if (spaceCount != 2)
		return false;
	while (ss >> token && tokenCount < 3) {
		tokens[tokenCount++] = token;
		token = "";
	}
	if (!token.empty())
		return false;
	
	requestLine.uri = tokens[1];
	requestLine.method = tokens[0];
	requestLine.httpversion = tokens[2];
	return     isValidMethod(requestLine.method)
			&& isValidURI(requestLine.uri)
			&& isValidHTTPVersion(requestLine.httpversion);
}

bool			Request::storeHeadersInVector() {
	size_t						rpos;
	std::string					line;
	std::string					toParse;
	size_t						opos = 0;
	
	toParse = extractHeadersFromBuffer();
	if (toParse.empty())
		return false;
	while (toParse[opos]) {
		rpos = toParse.find("\r\n", opos);
		if (rpos == std::string::npos)
			return false;
		line = toParse.substr(opos, rpos - opos);
		if (line.empty())
			break ;
		header.rawHeaders.push_back(line);
		opos = rpos + 2;
	}
	return true;
}

bool			Request::validateRequestHeaders() {
	bool			ContentLength = headerExists("Content-Length");
	bool			TransferEncoding = headerExists("Transfer-Encoding");

	if (ContentLength == TransferEncoding)
		return false;
	
	if (TransferEncoding && header.transferEncoding == "chunked")
		isEncoded = true;
	else if (TransferEncoding && header.transferEncoding != "chunked")
		return false; // NOT IMPLEMENTED

	if (ContentLength && body.contentLength == -1)
		return false;
	
	if (!headerExists("Host") || !headerExists("Connection"))
		return false;
	
	if (header.host.empty() || header.connection.empty())
		return false;

	int pos = header.contentType.find("multipart/form-data; boundary=") + 30;
	if (headerExists("Content-Type") && pos != 30)
		return false;
	else if (headerExists("Content-Type") && pos == 30 && header.contentType[pos])
		body.boundaryBegin = "--" + header.contentType.substr(pos), body.boundaryEnd = body.boundaryBegin + "--", isMultipart = true;

	return true;
}

// HEADERS CONTROL CENTER
bool			Request::parseRequestLineAndHeaders() {
	if (pState == HEADERS_FINISHED || !bufferContainHeaders()) {
		std::cout << "[REQUEST]\tParsing Headers POSTPONED OR FINISHED..." << std::endl;
		return true;
	}
	
	std::cout << "[REQUEST]\tParsing Headers..." << std::endl;
	if (!storeHeadersInVector())
		std::cout << "[ERROR!]\tstoreHeadersInVector();" << std::endl;
	if (!parseRequestLine())
		std::cout << "[ERROR!]\tparseRequestLine();" << std::endl;
	if (!parseHeaders())
		std::cout << "[ERROR!]\tparseHeaders();" << std::endl;
	// printParsedRequest();
	if (requestLine.method == "GET")
		pState = PARSING_FINISHED;
	if (!validateRequestHeaders())
		std::cout << "[ERROR!]\tvalidateRequestHeaders();" << std::endl;
	pState = HEADERS_FINISHED;
	return true;
}
