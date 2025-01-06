/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:33:50 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/04 13:08:22 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {
	statusCode = 200;
	
	buffer = "";
	bufferSize = 0;
	
	isEncoded = false;
	pState = PARSING_INIT;
	
	body.rawBody = "";
	body.boundaryBegin = "";
	body.boundaryEnd = "";
	body.bodySize = 0;
	body.contentLength = -1;

	header.host = "";
	header.contentType = "";
	header.connection = "";
	header.transferEncoding = "";
	// header.contentLength = "";

	requestLine.rawRequestLine = "";
	requestLine.method = "";
	requestLine.uri = "";
	requestLine.httpversion = "";
	// requestLine.query = "";

	// debugFD = open("DEBUG.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
	// rawBodyFD = open("REQUEST_BODY_DECODED.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
	// bufferFD = open("BUFFER.DEBUG", O_CREAT | O_RDWR | O_TRUNC, 0644);
}

Request::~Request() {
	
}

int&						Request::getStatusCode() {
	return this->statusCode;
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