/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 18:04:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 20:32:16 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() {
	isReady = false;
}

Response::~Response() {

}

void				Response::setRequest(Request* _Request) {
	this->_Request = _Request;
}

// void				Response::setConfig(const ServerConfig& _Config) {
// 	Config = _Config;
// }

void				Response::setvServerConfigs(const std::vector<ServerConfig>&			_vServerConfigs) {
	vServerConfigs = _vServerConfigs;
}

bool				Response::getResponseIsReady() {
	return isReady;
}

std::string&		Response::getRawResponse() {
	return rawResponse;
}
