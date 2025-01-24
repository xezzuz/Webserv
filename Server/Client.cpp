/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/24 20:10:30 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <fcntl.h>
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

std::string	queryString(std::string& uri) // protocol://domain/path/pathinfo?query#fragment
{
	std::string str;
	

	// remove the fragment part from the uri
	size_t pos = uri.find('#');
	if (pos != std::string::npos)
		uri.erase(pos);

	// saving the querystring and removing it from the uri
	pos = uri.find('?');
	if (pos != std::string::npos)
	{
		str = uri.substr(pos + 1);
		uri.erase(pos);
	}

	return (str);
}

void	validateUri(struct ResponseInput& input, std::string& location)
{
	struct stat targetStat;

	if (!rootJail(input.uri))
	{
		input.status = 403;
		return ;
	}

	input.queryString = queryString(input.uri);
	
	// replaces alias with location if not appends root
	if (!input.config.alias.empty())
		input.absolutePath = input.config.alias + input.uri.substr(location.length());
	else
	{
		if (input.config.root[input.config.root.length() - 1] == '/')
			input.config.root.erase(input.config.root.length() - 1);
		input.absolutePath = input.config.root + input.uri;
	}
	
	// checks for CGI and gets the interpreter path
	// for (std::map<std::string, std::string>::iterator it = input.config.cgi_ext.begin(); it != input.config.cgi_ext.end(); it++)
	// {
	// 	size_t pos = input.absolutePath.find(it->first);
	// 	if (pos != std::string::npos)
	// 	{
	// 		input.isCGI = true;
	// 		input.cgiExt = it->first;
	// 		input.cgiInterpreter = it->second;
			
	// 		// erase the path info from the absolute path
	// 		pos = input.absolutePath.find(pos, '/');
	// 		if (pos != std::string::npos)
	// 			input.absolutePath.erase(pos);
	// 	}
	// }

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