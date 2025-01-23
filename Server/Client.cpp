/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/23 17:34:25 by mmaila           ###   ########.fr       */
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

void	Client::reset()
{
	_Response = Response();
	_Request = Request();
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

// Directives&	matchingConfig(vServerConfig& server, std::string& uri)
// {
// 	std::string	location;
// 	std::map<std::string, Directives>::iterator it;

// 	for (it = server.Locations.begin(); it != server.Locations.end(); it++)
// 	{
// 		if (uri.find(it->first) != std::string::npos)
// 		{
// 			if (it->first.size() > location.size())
// 				location = it->first;
// 		}
// 	}
// 	if (location.empty())
// 		return (server.ServerDirectives);
// 	return (server.Locations.find(location)->second);
// }

void	validateUri(struct ResponseInput& input, std::string& location)
{
	struct stat targetStat;

	if (!rootJail(input.uri))
	{
		input.status = 403;
		return ;
	}


	if (!input.config.alias.empty())
		input.absolutePath = input.config.alias + input.uri.substr(location.length());
	else
	{
		if (input.config.root[input.config.root.length() - 1] == '/')
			input.config.root.erase(input.config.root.length() - 1);
		input.absolutePath = input.config.root + input.uri;
	}

	if (stat(input.absolutePath.c_str(), &targetStat) == -1)
	{
		input.status = 404;
		return ;
	}

	if (S_ISDIR(targetStat.st_mode))
	{
		if (access(input.absolutePath.c_str(), X_OK) != 0) // check exec permission to traverse dir
		{
			input.status = 403;
			return ;
		}
		input.isDir = true; // requested resource is a directory
		if (input.absolutePath.at(input.absolutePath.length() - 1) != '/')
			input.absolutePath.append("/");
	}
}

void	Client::initResponse(std::vector<vServerConfig>& servers)
{
	struct ResponseInput				input;

	input.method = _Request.getRequestLineSt().method;
	input.uri = _Request.getRequestLineSt().uri;
	input.status = _Request.getStatusCode();
	input.requestHeaders = _Request.getHeaderSt().headersMap;
	
	vServerConfig server = matchingServer(servers, _Request.getHeaderSt().host);

	std::string	location;
	std::map<std::string, Directives>::iterator it;
	for (it = server.Locations.begin(); it != server.Locations.end(); it++)
	{
		if (input.uri.find(it->first) != std::string::npos)
		{
			if (it->first.size() > location.size())
				location = it->first;
		}
	}
	if (location.empty())
		input.config = server.ServerDirectives;
	else
		input.config = server.Locations.find(location)->second;

	validateUri(input, location);
	
	// input.config.index.push_back("index.html");
	input.config.autoindex = true;
	_Response.setInput(input);
}