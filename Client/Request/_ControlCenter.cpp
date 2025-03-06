/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/06 20:35:40 by nazouz           ###   ########.fr       */
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
	
	std::map<std::string, Directives>::iterator it = matchingServer.Locations.begin();
	for (; it != matchingServer.Locations.end(); it++) {
		if (_RequestData.URI.find(it->first) == 0) {
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
