/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/19 19:58:26 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
	// std::cout << "Client::Constructor Called" << std::endl;
}

Client::Client(int socket) : socket(socket) {
	// std::cout << "Client::Param Constructor Called" << std::endl;
}

Client::Client(const Client& rhs)
{
	*this = rhs;
}


Client& Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		this->socket = rhs.socket;
		this->_Request = rhs._Request;
		this->_Response = rhs._Response;
	}
	return (*this);
}

Client::~Client() {
	// std::cout << "Client::Destructor Called" << std::endl;
}

Request&		Client::getRequest() {
	return _Request;
}

Response&		Client::getResponse() {
	return _Response;
}

int				Client::getSocket() {
	return socket;
}

void	Client::resetResponse()
{
	_Response = Response();
}

void	Client::initResponse(std::vector<std::map<std::string, Directives>>& servers)
{
	struct ResponseInput	input;
	vServerConfig			responsibleServer;
	std::string				responsibleLocation;

	input.method = _Request.getRequestLineSt().method;
	input.uri = _Request.getRequestLineSt().uri;
	input.status = _Request.getStatusCode();
	input.requestHeaders = _Request.getHeaderSt().headersMap;

	//find matching virtual server based on host header on request
	for (size_t i = 0; i < vServerConfigs.size(); i++)
	{
		if (std::find(vServerConfigs[i].baseConfig["server_name"].begin(), vServerConfigs[i].baseConfig["server_name"].end(), _Request.getHeaderSt().host) != vServerConfigs[i].baseConfig["server_name"].end())
			responsibleServer = vServerConfigs[i];
	}
	std::cout << "no matching server block was found! falling back to default server" << std::endl;
	responsibleServer = vServerConfigs[0];
	
	// find matching location block
	for (Location::iterator it = responsibleServer.location.begin(); it != responsibleServer.location.end(); it++)
	{
		if (input.uri.find(it->first) != std::string::npos)
		{
			if(it->first.size() > responsibleLocation.size())
				responsibleLocation = it->first;
		}
	}
	
	// replace alias
	if (!responsibleLocation.empty())
	{
		if (responsibleServer.location[responsibleLocation].find("alias") != responsibleServer.location[responsibleLocation].end())
			input.uri.replace(0, responsibleServer.location[responsibleLocation]["alias"][0].length(), responsibleServer.location[responsibleLocation]["alias"][0]);
	}
	
	
	// insert configs into ResponseInput
	std::map<std::string, std::vector<std::string> >::iterator itBase = responsibleServer.baseConfig.begin();
	for (itBase = responsibleServer.baseConfig.begin(); itBase != responsibleServer.baseConfig.end(); itBase++)
	{
		if (!responsibleLocation.empty())
		{
			std::map<std::string, std::vector<std::string> >::iterator itDirctive = responsibleServer.location[responsibleLocation].find(itBase->first);
			if (!responsibleLocation.empty() && itDirctive != responsibleServer.location[responsibleLocation].end())
			{
				input.config.insert(*itDirctive);
				continue ;
			}
		}
		input.config.insert(*itBase);
	}
	if (input.config.root[input.config.root.length() - 1] == '/')
		input.config.root.erase(input.config.root.length() - 1);
	_Response.setInput(input);
}