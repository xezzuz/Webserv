/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/26 16:08:16 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

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

// PARSING CONTROL CENTER
int					Request::parseControlCenter(char *recvBuffer, int recvBufferSize)
{
	buffer.append(recvBuffer, recvBufferSize);
	bufferSize += recvBufferSize;
	std::cout << "------REQUEST " << recvBufferSize << "-----" << std::endl;
	std::cout << buffer;
	std::cout << "------------------" << std::endl;
	
	if (!headersFinished) {
		if (buffer.find("\r\n\r\n") == std::string::npos)
			return RECV;
		parseRequestLine();
		parseHeaders();
		validateRequestHeaders();
		setMatchingConfig();
		fillRequestData(_RequestData.URI, _RequestData);
		headersFinished = true;
		if (_RequestData.Method != "POST")
			return RESPOND;
		else if (_RequestData.isCGI && !isEncoded)
			return FORWARD_CGI;
	}
	parseRequestBody();
	if (bodyFinished)
		return RESPOND;
	return RECV;
}
