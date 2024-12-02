/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 18:39:51 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/02 19:39:55 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void				Response::setResponsibleConfig() {
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

void				Response::setRequestedResource() {
	std::string			uri = _Request->getRequestLineSt().uri;

	requestedResource = locationBlock->root;
	requestedResource += uri.substr(locationBlock->location.size());
}

bool				Response::isMethodAllowed() {
	for (size_t i = 0; i < locationBlock->methods.size(); i++) {
		if (_Request->getRequestLineSt().method == locationBlock->methods[i])
			return true;
	}
	return false;
}

bool				Response::generateResponse() {
	statusCode = _Request->getStatusCode();
	setResponsibleConfig();
	setMatchingLocationBlock();
	setRequestedResource();
	
	// statusLine.httpversion = "HTTP/1.1";
	// std::stringstream ss;

	// ss << _Request->getStatusCode();
	// statusLine.status_code = ss.str();
	// statusLine.reason_phrase = "OK";
	// // statusLine.reason_phrase = getReasonPhrase(statusLine.status_code);
	
	// rawResponse = statusLine.httpversion + " " + statusLine.status_code + " " + statusLine.reason_phrase + "\r\n";
	// rawResponse += "Connection: close\r\n\r\n";
	// rawResponse += "<p>" + statusLine.status_code + " " + statusLine.reason_phrase + "</p>";
	// isReady = true;


	/*
		if locations.size() == 0,  we should add a generic location '/'
		if no location block is matching, 404 Not Found
	*/

	if (locationHasRedirection()) {
		std::cout << "HTTP/1.1 " << locationBlock->redirect[0] << " " << "Moved Permanently" << std::endl;
		std::cout << "Location: " << locationBlock->redirect[1] << std::endl;
		// sendResponse();
	}
	if (!isMethodAllowed()) {
		std::cout << "HTTP/1.1 " << 405 << " " << "Method Not Allowed" << std::endl;
		std::cout << "Method: " << _Request->getRequestLineSt().method << " is not allowed!" << std::endl;
		// you should response with Allowed header
		// sendResponse();
	}
	std::string tmp = _Request->getRequestLineSt().uri.substr(locationBlock->location.size());
	std::cout << "the client requested: " << locationBlock->root + tmp << std::endl;
	handleResponseByMethod();
	return true;
}

void				Response::handleResponseByMethod() {
	std::string			method = _Request->getRequestLineSt().method;
	
	if (method == "GET")
		handleGET();
	// else if (method == "POST")
	// 	handlePOST();
	// else if (method == "DELETE")
	// 	handleDELETE();
}
