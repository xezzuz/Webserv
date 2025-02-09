/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/09 15:28:11 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "../Response/Error.hpp"

void			Request::setStatusCode(int code) {
	_RequestData.StatusCode = code;
	// if (code != 200)
	// 	pState = REQUEST_FINISHED;
}

void			Request::setRequestState() {
	if (REQUEST_INIT == pState && bufferContainHeaders())
		pState = REQUEST_HEADERS;
	else if (REQUEST_HEADERS == pState)
		pState = REQUEST_FINISHED;
}

ServerConfig&				Request::getMatchingServer() {
	for (size_t i = 0; i < vServers.size(); i++) {
		for (size_t j = 0; j < vServers[i].server_names.size(); j++) {
			if (vServers[i].server_names[j] == _RequestData.host)
				return vServers[i];
		}
	}
	return vServers[0];
}

void						Request::setMatchingConfig() {
	ServerConfig&			matchingServer = getMatchingServer();
	
	// this is implemented by mmaila, i should check if its working properly with prefix matching
	std::map<std::string, Directives>::iterator it = matchingServer.Locations.begin();
	for (; it != matchingServer.Locations.end(); it++) {
		if (_RequestData.URI.find(it->first) != std::string::npos) {
			if (it->first.size() > _RequestData.matchingLocation.size())
				_RequestData.matchingLocation = it->first;
		}
	}

	_RequestData._Config = (_RequestData.matchingLocation.empty()) ? 
	&matchingServer.ServerDirectives : &matchingServer.Locations.find(_RequestData.matchingLocation)->second;
}

// FEED REQUEST
int				Request::feedRequest(int clientSocket) {
	char	buf[REQUEST_BUFFER_SIZE] = {0};

	int	bytesReceived = recv(clientSocket, buf, REQUEST_BUFFER_SIZE, 0);
	if (bytesReceived > 0) {
		std::cout << "----------REQUEST_OF_CLIENT " << clientSocket << "----------" << std::endl;
		std::cout << buf;
		std::cout << "---------------------------------------------------------" << std::endl;
		buffer += std::string(buf, bytesReceived);
		bufferSize += bytesReceived;
		parseControlCenter();
	}
	else if (bytesReceived == 0) { // this is for graceful shutdown (client closes the connection willingly)
		throw(Disconnect("[CLIENT-" + _toString(clientSocket) + "] CLOSED CONNECTION"));
	}
	else {
		throw(Disconnect("[CLIENT-" + _toString(clientSocket) + "] recv: " + strerror(errno)));
	}
	return (pState);
}

// PARSING CONTROL CENTER
bool			Request::parseControlCenter() {
	setRequestState();
	switch (pState) {
		case REQUEST_HEADERS:
			parseRequestLineAndHeaders();
			break;
		default:
			break;
	}
	setRequestState();
	return true;
}
// void			Request::feedRequest(char *recvBuffer, int recvBufferSize) {
// 	std::string		recvBuff(recvBuffer, recvBufferSize);
// 	buffer.append(recvBuffer, recvBufferSize);
// 	bufferSize += recvBufferSize;
// 	parseControlCenter();
// 	std::cout << pState << std::endl;
// }
