/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:26:22 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/13 18:25:27 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void			Request::printParsedRequest() {
	printf("/---------------- REQUEST ----------------/\n\n");
	for (size_t i = 0; i < rawRequest.size(); i++) {
		printf("[%s]\n", rawRequest[i].c_str());
	}

	// startline
	printf("/---------------- STARTLINE ----------------/\n");
	printf("RAW STARTLINE: [%s]\n", requestLine.rawRequestLine.c_str());
	printf("METHOD: [%s]\n", requestLine.method.c_str());
	printf("URI: [%s]\n", requestLine.uri.c_str());
	printf("HTTP VERSION: [%s]\n\n", requestLine.httpversion.c_str());

	// header
	printf("/----------------- HEADERS -----------------/\n");
	printf("RAW HEADERS: [%s]\n", header.rawHeader.c_str());
	std::map<std::string, std::string>::iterator it = header.headersMap.begin();
	std::map<std::string, std::string>::iterator ite = header.headersMap.end();
	while (it != ite) {
		printf("[%s][%s]\n", it->first.c_str(), it->second.c_str());
		it++;
	}
	
	// body
	printf("\n/------------------- BODY -------------------/\n");
	printf("RAW BODY: [%s]\n", body.rawBody.c_str());
}

Request::Request() {

}

Request::Request(std::string rawRequest) {
	if (!parseRequest(rawRequest)) {
		printf("Invalid Request!!!\n");
	}
	printParsedRequest();
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
	std::stringstream	ss(requestLine.rawRequestLine);

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
	
	requestLine.method = tokens[0];
	requestLine.uri = tokens[1];
	requestLine.httpversion = tokens[2];
	return isValidMethod(requestLine.method)
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
	// should check if its a valid integer or not
	// it does throw an exception maybe?
	it = header.headersMap.find("Content-Length");
	if (it != header.headersMap.end())
		body.contentLength = std::atoi(it->second.c_str());
	printf("Headers Are Valid!\n");
	return true;
}


/*
	The presence of a message body in a request is signaled
	by a Content-Length or Transfer-Encoding header field.
*/
// if both are present TE is first
// if TE is not chunked && CL is valid => ignore TE
bool			Request::parseBody() {
	// bool		pContentLength = 
	// 	header.headersMap.find("Content-Length") != header.headersMap.end();
	// bool		pTransferEncoding = 
	// 	header.headersMap.find("Transfer-Encoding") != header.headersMap.end();

	// if (pContentLength && pTransferEncoding)
	// 	return false;
	// if (pTransferEncoding && header.transferEncoding == "chunked")
	// 	return parseChunks();
	// else if (pTransferEncoding && header.transferEncoding != "chunked")
	// 	return false;
	// if (pContentLength)
	// 	return parseBody();
	// return true;
	return true;
}

bool			Request::parseRequest(const std::string& _rawRequest) {
	// split the lines using CRLF delim
	size_t						rpos;
	std::string					line;
	size_t						opos = 0;

	while (_rawRequest[opos]) {
		rpos = _rawRequest.find("\r\n", opos);
		if (rpos == std::string::npos)
			return false;
		line = _rawRequest.substr(opos, rpos - opos);
		if (line.empty())
			break ;
		rawRequest.push_back(line);
		opos = rpos + 2;
	}
	if ((opos = rpos + 2) && _rawRequest[opos])
		body.rawBody = _rawRequest.substr(opos);
	
	requestLine.rawRequestLine = rawRequest[0];
	return parseRequestLine()
			&& parseHeaders();
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
