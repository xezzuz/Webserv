/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/21 21:18:51 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "../Response/Error.hpp"

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
bool	Request::parseControlCenter(char *recvBuffer, int recvBufferSize)
{
	buffer.append(recvBuffer, recvBufferSize);
	bufferSize += recvBufferSize;
	std::cout << "------REQUEST-----" << std::endl;
	std::cout << buffer;
	std::cout << "------------------" << std::endl;

	if (buffer.find("\r\n\r\n") == std::string::npos)
		return (false);

	parseRequestLine();
	parseHeaders();
	validateRequestHeaders();
	setMatchingConfig();
	fillRequestData(_RequestData.URI, _RequestData);
	return (true);
}
// void			Request::feedRequest(char *recvBuffer, int recvBufferSize) {
// 	std::string		recvBuff(recvBuffer, recvBufferSize);
// 	buffer.append(recvBuffer, recvBufferSize);
// 	bufferSize += recvBufferSize;
// 	parseControlCenter();
// 	std::cout << pState << std::endl;
// }
