/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/27 22:38:05 by mmaila           ###   ########.fr       */
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

std::string	queryString(std::string& path) // protocol://domain/path/script.cgi/pathinfo?query#fragment
{
	std::string str;
	
	// remove the fragment part from the uri
	size_t pos = path.find('#');
	if (pos != std::string::npos)
		path.erase(pos);

	// extracts the query string from the uri
	pos = path.find('?');
	if (pos != std::string::npos)
	{
		str = path.substr(pos + 1);
		path.erase(pos);
	}
	return (str);
}

// void	validateUri(struct ResponseInput& input, std::string& location)
// {
// 	struct stat targetStat;

// 	if (!rootJail(input.uri))
// 	{
// 		input.status = 403;
// 		return ;
// 	}

// 	input.queryString = queryString(input.uri);
	
// 	// replaces alias with location if not appends root
// 	if (!input.config.alias.empty())
// 		input.absolutePath = input.config.alias + input.uri.substr(location.length());
// 	else
// 	{
// 		if (input.config.root[input.config.root.length() - 1] == '/')
// 			input.config.root.erase(input.config.root.length() - 1);
// 		input.absolutePath = input.config.root + input.uri;
// 	}
	
// 	// checks for CGI and gets the interpreter path
// 	// for (std::map<std::string, std::string>::iterator it = input.config.cgi_ext.begin(); it != input.config.cgi_ext.end(); it++)
// 	// {
// 	// 	size_t pos = input.absolutePath.find(it->first);
// 	// 	if (pos != std::string::npos)
// 	// 	{
// 	// 		input.isCGI = true;
// 	// 		input.cgiExt = it->first;
// 	// 		input.cgiInterpreter = it->second;
			
// 	// 		// erase the path info from the absolute path
// 	// 		pos = input.absolutePath.find(pos, '/');
// 	// 		if (pos != std::string::npos)
// 	// 			input.absolutePath.erase(pos);
// 	// 	}
// 	// }

// 	if (stat(input.absolutePath.c_str(), &targetStat) == -1)
// 	{
// 		input.status = 404;
// 		return ;
// 	}

// 	if (S_ISDIR(targetStat.st_mode))
// 	{
// 		if (access(input.absolutePath.c_str(), X_OK) != 0) // check exec permission to traverse dir
// 		{
// 			input.status = 403;
// 			return ;
// 		}
// 		input.isDir = true; // requested resource is a directory
// 		if (input.absolutePath.at(input.absolutePath.length() - 1) != '/')
// 			input.absolutePath.append("/");
// 	}
// }

void	Client::decodeUri(struct ResponseInput& input)
{	
	if (!rootJail(input.uri))
	{
		input.status = 403;
		return ;
	}

	struct stat pathStat;
	size_t 		pos;
	std::string	URL;

	pos = input.absolutePath.find_last_of('#');
	if (pos != std::string::npos)
		input.absolutePath.erase(pos);
	pos = input.absolutePath.find_last_of('?');
	if (pos != std::string::npos)
	{
		input.cgi.queryString = input.absolutePath.substr(pos + 1);
		input.absolutePath.erase(pos);
	}

	std::string path;
	
	while (!input.absolutePath.empty())
	{
		pos = input.absolutePath.find('/', 1);
		path.append(input.absolutePath.substr(0, pos));
		input.absolutePath.erase(0, pos++);
		if (stat(path.c_str(), &pathStat) == -1)
		{
			input.status = 404;
			return ;
		}
		if (!S_ISDIR(pathStat.st_mode))
			break;
	}
	
	if (S_ISDIR(pathStat.st_mode))
	{
		if (access(path.c_str(), X_OK) != 0)
		{
			input.status = 403;
			return ;
		}
		input.isDir = true;
		if (input.method == "GET")
		{
			std::vector<std::string>::iterator	it;

			for (it = input.config.index.begin(); it != input.config.index.end(); it++)
			{
				if (access((path + *it).c_str(), F_OK) == 0) // file exists
				{
					path.append(*it);
					input.isDir = false;
					break;
				}
			}
			if (it == input.config.index.end())
			{
				if (!input.config.autoindex)
					input.status = 404;
				return ;
			}
		}
	}

	if (!input.isDir)
	{	
		if (access(path.c_str(), R_OK) != 0)
		{
			input.status = 403;
			return ;
		}

		std::map<std::string, std::string>::iterator it;
		input.cgi.scriptName = path.substr(path.find_last_of('/'));
		if ((pos = input.cgi.scriptName.find('.')) != std::string::npos)
		{
			std::map<std::string, std::string>::iterator it = input.config.cgi_ext.find(input.cgi.scriptName.substr(pos));
			if (it != input.config.cgi_ext.end())
			{
				input.cgi.isCgi = true;
				input.cgi.ext = it->first;
				input.cgi.interpreter = it->second;
				input.cgi.pathInfo = input.absolutePath;
				input.absolutePath = path;
			}
			else if (input.absolutePath.size() > 0)
			{
				input.status = 404;
				return ;
			}
		}
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

	// alias and root appending
	if (!input.config.alias.empty())
		input.absolutePath = input.config.alias + input.uri.substr(location.length());
	else
	{
		if (input.config.root[input.config.root.length() - 1] == '/') // do same for alias
			input.config.root.erase(input.config.root.length() - 1);
		input.absolutePath = input.config.root + input.uri;
	}

	// validateUri(input, location);
	input.config.cgi_ext.insert(std::make_pair(".py", "/usr/bin/python3"));
	input.config.cgi_ext.insert(std::make_pair(".php", "/usr/bin/php"));
	decodeUri(input);
	std::cout << "FINAL PATH : " << input.absolutePath << std::endl;
	// input.config.index.push_back("index.html");
	input.config.autoindex = true;
	_Response.setInput(input);
}