/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:26:22 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/17 19:58:30 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

bool			Request::printParsedRequest() {
	std::cout << "*********************************************" << std::endl;
	// printf("/---------------- REQUEST ----------------/\n\n");
	// for (size_t i = 0; i < rawRequest.size(); i++) {
	// 	printf("[%s]\n", rawRequest[i].c_str());
	// }

	// startline
	printf("/---------------- REQUESTLINE ----------------/\n");
	// printf("RAW STARTLINE: [%s]\n", requestLine.rawRequestLine.c_str());
	printf("METHOD: [%s]\n", requestLine.method.c_str());
	printf("URI: [%s]\n", requestLine.uri.c_str());
	printf("HTTP VERSION: [%s]\n\n", requestLine.httpversion.c_str());

	// header
	printf("/----------------- HEADERS -----------------/\n");
	// printf("RAW HEADERS: [%s]\n", header.rawHeader.c_str());
	std::map<std::string, std::string>::iterator it = header.headersMap.begin();
	std::map<std::string, std::string>::iterator ite = header.headersMap.end();
	while (it != ite) {
		printf("[%s][%s]\n", it->first.c_str(), it->second.c_str());
		it++;
	}
	
	// body
	// printf("\n/------------------- BODY -------------------/\n");
	// printf("RAW BODY: [%s]\n", body.rawBody.c_str());
	std::cout << "*********************************************" << std::endl;
	return true;
}

Request::Request() {
	// std::cout << "Default Constructor Called" << std::endl;
}

Request::Request(const std::string& rawRequest) : buffer(rawRequest) {
	statusCode = 0;
	
	bodyParsed = false;
	headersParsed = false;
	parsingFinished = false;
	
	body.rawBody = "";
	body.boundaryBegin = "";
	body.boundaryEnd = "";
	body.bodySize = -1;
	body.contentLength = -1;

	header.rawHeader = "";
	header.host = "";
	header.contentType = "";
	header.connection = "";
	header.transferEncoding = "";
	header.contentLength = "";

	requestLine.rawRequestLine = "";
	requestLine.method = "";
	requestLine.uri = "";
	requestLine.httpversion = "";
	// requestLine.query = "";
	
}

Request::~Request() {

}


bool			isValidMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		return false;
	return true;
}

/*
	- URI Length max = 2048
	- Invalid Characters "-._~:/?#[]@!$&'()*+,;=%"
*/

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

bool			Request::parseRequestLine() {
	std::string			token;
	std::string			tokens[3];
	size_t				tokenCount = 0;
	size_t				spaceCount = 0;
	std::stringstream	ss(rawRequest[0]);

	requestLine.rawRequestLine = rawRequest[0];
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
	if (requestLine.method == "GET")
		bodyParsed = true;
	return     isValidMethod(requestLine.method)
			&& isValidURI(requestLine.uri)
			&& isValidHTTPVersion(requestLine.httpversion);
}

bool			isValidFieldLine(const std::string& fieldline) {
	std::string			line(fieldline);
	size_t				colonPos;

	colonPos = line.find(':');
	if (colonPos == std::string::npos)
		return false;
	
	std::string			fieldName = line.substr(0, colonPos);
	if (fieldName.empty())
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

	for (size_t i = 1; i < rawRequest.size(); i++) {
		if (rawRequest[i].empty())
			break ;
		if (!isValidFieldLine(rawRequest[i]))
			return false;
		int colonPos = rawRequest[i].find(':');
		fieldname = rawRequest[i].substr(0, colonPos);
		fieldvalue = stringtrim(rawRequest[i].substr(colonPos + 1));
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

bool			Request::parseChunkedBody() {
	size_t			cpos = 0;
	size_t			CRLFpos = 0;
	std::string		chunkSizeHex;
	
	if (!chunkReceived())
		return true;

	CRLFpos = buffer.find("\r\n", cpos);
	if (CRLFpos == std::string::npos)
		return false;
	chunkSizeHex = buffer.substr(cpos, CRLFpos - cpos);
	if (!isHexa(chunkSizeHex))
		return false;
		
	int	chunkSize = hexToInt(chunkSizeHex);
	if (!chunkSize && buffer.substr(CRLFpos, 4) == "\r\n\r\n")
		return true;
	cpos = CRLFpos + 2;
	
	if (buffer.size() < cpos + chunkSize + 2)
		return false;
	body.rawBody += buffer.substr(cpos, chunkSize);
	cpos += chunkSize + 2;
	if (buffer.substr(cpos - 2, 2) != "\r\n")
		return false;
	buffer = buffer.substr(cpos);
	
	// buffer = "";
	// bodyParsed = true;
	return true;
}

bool			Request::parseLengthBody() {
	if (buffer.size() < body.contentLength)
		return true;
	body.rawBody = buffer.substr(0, body.contentLength);
	bodyParsed = true;
	return true;
}

bool			Request::parseRequestBody() {
	bool			ContentLength = headerExists("Content-Length");
	bool			TransferEncoding = headerExists("Transfer-Encoding");
	
	if (bodyParsed)
		return true;
	
	if (!ContentLength && !TransferEncoding)
		return false;
	
	if (TransferEncoding && header.transferEncoding == "chunked")
		return parseChunkedBody();
	else if (TransferEncoding && header.transferEncoding != "chunked")
		return false;
	
	if (ContentLength && body.contentLength > 0)
		return parseLengthBody();
	else if (ContentLength && body.contentLength < 0)
		return false;
	
	return false;
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
		rawRequest.push_back(line);
		opos = rpos + 2;
	}
	return true;
}

bool			Request::parseRequestLineAndHeaders() {
	if (headersParsed || !headersReceived())
		return true;
	
	storeHeadersInVector();
	parseRequestLine();
	parseHeaders();
	headersParsed = true;
}

// CONTROL CENTER
bool			Request::requestParseControl() {
	parseRequestLineAndHeaders();
	parseRequestBody();
	parsingFinished = true;
}

bool			Request::headerExists(const std::string& key) {
	std::map<std::string, std::string>::iterator it;
	it = header.headersMap.find(key);
	if (it == header.headersMap.end())
		return false;
	return true;
}

bool			Request::headersReceived() {
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
	return header;
}

bool			Request::chunkReceived() {
	size_t			cpos = 0;
	size_t			crlfpos = 0;
	std::string		chunkSizeStr;

	crlfpos = buffer.find("\r\n");
	if (crlfpos == std::string::npos)
		return false;
	
	chunkSizeStr = buffer.substr(cpos, crlfpos - cpos);
	if (!isHexa(chunkSizeStr))
		return false;
	
	if (buffer.find("\r\n", crlfpos + 2) != std::string::npos)
		return true;
	return false;
}
























std::vector<std::string>&	Request::getRawRequest() {
	return this->rawRequest;
}

t_body&						Request::getBodySt() {
	return this->body;
}

t_header&					Request::getHeaderSt() {
	return this->header;
}

t_requestline&				Request::getRequestLineSt() {
	return this->requestLine;
}
