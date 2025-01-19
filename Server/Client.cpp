/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/19 20:30:03 by mmaila           ###   ########.fr       */
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

vServerConfig&	matchingServer(std::vector<vServerConfig>& servers, std::string& host)
{
	for (std::vector<vServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->host == host)
			return (*it);
	}
	return (servers[0]);
}

Directives&	matchingConfig(vServerConfig server, std::string& uri)
{
	std::string	location;
	std::map<std::string, Directives>::iterator it;

	for (it = server.Locations.begin(); it != server.Locations.end(); it++)
	{
		if (uri.find(it->first) != std::string::npos)
		{
			if (it->first.size() > location.size())
				location = it->first;
		}
	}
	if (location.empty())
		return (server.ServerDirectives);
	return (server.Locations.find(location)->second);
}

void	Client::initResponse(std::vector<vServerConfig>& servers)
{
	struct ResponseInput	input;

	input.method = _Request.getRequestLineSt().method;
	input.uri = _Request.getRequestLineSt().uri;
	input.status = _Request.getStatusCode();
	input.requestHeaders = _Request.getHeaderSt().headersMap;
	input.config = matchingConfig(matchingServer(servers, _Request.getHeaderSt().host), input.uri);
	
	if (!input.config.alias.empty())
		input.uri.replace(0, input.config.alias.length(), input.config.alias); // buggy
	
	if (input.config.root[input.config.root.length() - 1] == '/')
		input.config.root.erase(input.config.root.length() - 1);

	_Response.setInput(input);
}