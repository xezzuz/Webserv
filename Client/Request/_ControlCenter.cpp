/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/05 01:57:11 by mmaila           ###   ########.fr       */
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
		std::cout << "LOCATIONS: " << it->first << std::endl;
		if (_RequestData.URI.find(it->first) != std::string::npos) {
			std::cout << "MATCHING LOCATION: " << it->first << std::endl;
			if (it->first.size() > _RequestData.matchingLocation.size())
				_RequestData.matchingLocation = it->first;
		}
	}

	_RequestData._Config = (_RequestData.matchingLocation.empty()) ? 
	&matchingServer.ServerDirectives : &matchingServer.Locations.find(_RequestData.matchingLocation)->second;
	
	if (std::find(_RequestData._Config->methods.begin(), _RequestData._Config->methods.end(), _RequestData.Method) == _RequestData._Config->methods.end())
		throw Code(405);
}

// PARSING CONTROL CENTER
int					Request::parseControlCenter(char *recvBuffer, int recvBufferSize) {
	buffer.append(recvBuffer, recvBufferSize);

	if (!headersFinished) {
		if (buffer.size() > 32 * KB)
			throw Code(400);
		if (buffer.find(DOUBLE_CRLF) == std::string::npos)
			return RECV;

		std::cout << "================RECIEVED=============" << std::endl;
		std::cout << buffer.substr(0, buffer.find("\r\n\r\n"));
		std::cout << "====================================" << std::endl;
		
		parseRequestLineAndHeaders();
		setMatchingConfig();
		resolveURI(_RequestData);
		if (_RequestData.Method != "POST")
			return RESPOND; // stop receiving
		if (_RequestData.isCGI) {
			if (!isEncoded)
				return FORWARD_CGI;
			setupCGITempFile();
		}
	}
	parseRequestBody();
	return bodyFinished;
}
