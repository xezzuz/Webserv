/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:33:50 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/27 18:29:12 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::~Request() {}

Request::Request(std::vector<ServerConfig>&	vServers) : vServers(vServers) {
	_RequestData.StatusCode = 200;
	bufferSize = 0;
	
	_RequestData._Config = &vServers[0].ServerDirectives;
	isEncoded = false;
	isMultipart = false;
	headersFinished = false;
	bodyFinished = false;
	
	_RequestRaws.bodySize = 0;
	_RequestData.contentLength = -1;

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
		headersFinished = rhs.headersFinished;
		bodyFinished = rhs.bodyFinished;
		isEncoded = rhs.isEncoded;
		isMultipart = rhs.isMultipart;
		// statusCode = rhs.statusCode;
	}
	return (*this);
}

// void	Request::receive()
// {
// 	char	buf[RECV_BUFFER_SIZE] = {0};

// 	ssize_t	bytesReceived = recv(socket, buf, RECV_BUFFER_SIZE, 0);
// 	if (bytesReceived > 0)
// 	{
// 		buffer += std::string(buf, bytesReceived);
// 		bufferSize += bytesReceived;
// 		parseControlCenter();
// 		return (pState);
// 	}
// 	else if (bytesReceived == 0)
// 		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
// 	else
// 		throw(Disconnect("[CLIENT-" + _toString(socket) + "] recv: " + strerror(errno)));
// }

// int&						Request::getStatusCode() {
// 	return this->statusCode;
// }
