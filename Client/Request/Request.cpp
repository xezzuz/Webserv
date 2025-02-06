/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:33:50 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/05 16:56:09 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::~Request() {
	// free DIR*
}

Request::Request(std::vector<ServerConfig>&	vServers) : vServers(vServers) {
	statusCode = 200;
	bufferSize = 0;
	
	isEncoded = false;
	isMultipart = false;
	pState = PARSING_INIT;
	
	_RequestRaws.bodySize = 0;
	_RequestRaws.contentLength = -1;

	_RequestData._Config = NULL;
	// debugFD = open("DEBUG.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
	// rawBodyFD = open("REQUEST_BODY_DECODED.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
	// bufferFD = open("BUFFER.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
}

Request::Request(const Request& rhs) : vServers(rhs.vServers) {
	*this = rhs;
}

Request&	Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		buffer = rhs.buffer;
		bufferSize = rhs.bufferSize;
		files = rhs.files;

		// new
		_RequestData = rhs._RequestData;
		_RequestRaws = rhs._RequestRaws;

		// body = rhs.body;
		// header = rhs.header;
		// requestLine = rhs.requestLine;
		isEncoded = rhs.isEncoded;
		isMultipart = rhs.isMultipart;
		pState = rhs.pState;
		statusCode = rhs.statusCode;
	}
	return (*this);
}

int&						Request::getStatusCode() {
	return this->statusCode;
}

// t_body&						Request::getBodySt() {
// 	return this->body;
// }

// t_header&					Request::getHeaderSt() {
// 	return this->header;
// }

// t_requestline&				Request::getRequestLineSt() {
// 	return this->requestLine;
// }
