/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 18:04:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/02 19:34:04 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() {
	isReady = false;
}

Response::Response(const Response& original) {
	*this = original;
}

Response&		Response::operator=(const Response& original) {
	if (this != &original) {
		isReady = original.isReady;
		statusCode = original.statusCode;
		requestedResource = original.requestedResource;
		_Request = original._Request;
		_Config = original._Config;
		vServerConfigs = original.vServerConfigs;
		locationBlock = original.locationBlock;
		statusLine = original.statusLine;
		rawResponse = original.rawResponse;
	}
	return *this;
}

Response::~Response() {

}

void				Response::setRequest(Request* _Request) {
	this->_Request = _Request;
}

// void				Response::setConfig(const ServerConfig& _Config) {
// 	Config = _Config;
// }

void				Response::setvServerConfigs(const std::vector<ServerConfig>&	_vServerConfigs) {
	vServerConfigs = _vServerConfigs;
}

bool				Response::getResponseIsReady() {
	return isReady;
}

std::string&		Response::getRawResponse() {
	return rawResponse;
}
