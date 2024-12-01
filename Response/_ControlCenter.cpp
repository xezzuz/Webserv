/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 18:39:51 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 20:47:29 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void			Response::feedResponse(Request* _Request) {
	this->_Request = _Request;

	if (!generateResponse()) {
		std::cout << "error generating response!" << std::endl;
	}
}

void			Response::setResponsibleConfig() {
	std::string			host = _Request->getHeaderSt().host;
	
	for (size_t i = 0; i < vServerConfigs.size(); i++) {
		for (size_t j = 0; j < vServerConfigs[i].server_name.size(); j++) {
			if (vServerConfigs[i].server_name[j] == host) {
				_Config = vServerConfigs[i];
				std::cout << "matching server block was found! " << i << std::endl;
				return ;
			}
		}
	}
	_Config = vServerConfigs[0];
	std::cout << "no matching server block was found! falling back to default server" << std::endl;
}

bool			Response::generateResponse() {
	statusLine.httpversion = "HTTP/1.1";
	std::stringstream ss;

	ss << _Request->getStatusCode();
	statusLine.status_code = ss.str();
	statusLine.reason_phrase = "OK";
	// statusLine.reason_phrase = getReasonPhrase(statusLine.status_code);
	
	rawResponse = statusLine.httpversion + " " + statusLine.status_code + " " + statusLine.reason_phrase + "\r\n";
	rawResponse += "Connection: close\r\n\r\n";
	rawResponse += "<p>" + statusLine.status_code + " " + statusLine.reason_phrase + "</p>";
	isReady = true;
	return true;
}
