/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConstructor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:55:44 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 20:08:12 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::constructServers() {
	for (size_t i = 0; i < Parser.size(); i++) {
		ServerConfig		newServer;

		fillServerConfigByParserDirectives(Parser[i].serverDirectives, newServer);
		for (size_t j = 0; Parser[i].locationDirectives.size(); j++) {
			LocationConfig		newLocation;
			fillLocationConfigByParserDirectives(Parser[i].serverDirectives, Parser[i].locationDirectives[j], newServer);
		}
	}
	return true;
}

bool				Config::fillServerConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, ServerConfig& Server) {

	if (serverDirectives.find("port") != serverDirectives.end())
		Server.port = std::atoi(serverDirectives["port"].c_str());
	else
		Server.port = std::atoi(defaultServerDirectives["port"].c_str());
	
	if (serverDirectives.find("host") != serverDirectives.end())
		Server.host = serverDirectives["host"];
	else
		Server.host = defaultServerDirectives["host"];
	
	std::string			temp_server_name;
	if (serverDirectives.find("server_name") != serverDirectives.end())
		temp_server_name = serverDirectives["server_name"];
	else
		temp_server_name = defaultServerDirectives["server_name"];
	
	std::string			token;
	std::stringstream	ss(temp_server_name);
	while (ss >> token)
		Server.server_name.push_back(token);
	
	return true;
}

bool				Config::fillLocationConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, std::map<std::string, std::string>& locationDirectives, ServerConfig& Server) {
	LocationConfig			newLocation;
	
	newLocation.location = locationDirectives["location"];
	
	if (locationDirectives.find("root") != locationDirectives.end())
		newLocation.root = locationDirectives["root"];
	else if (serverDirectives.find("root") != serverDirectives.end())
		newLocation.root = serverDirectives["root"];
	else if (defaultServerDirectives.find("root") != defaultServerDirectives.end())
		newLocation.root = defaultServerDirectives["root"];
	
	if (locationDirectives.find("upload_store") != locationDirectives.end())
		newLocation.root = locationDirectives["upload_store"];
	else if (serverDirectives.find("upload_store") != serverDirectives.end())
		newLocation.root = serverDirectives["upload_store"];
	else if (defaultServerDirectives.find("upload_store") != defaultServerDirectives.end())
		newLocation.root = defaultServerDirectives["upload_store"];
	
	if (locationDirectives.find("autoindex") != locationDirectives.end())
		newLocation.root = locationDirectives["autoindex"];
	else if (serverDirectives.find("autoindex") != serverDirectives.end())
		newLocation.root = serverDirectives["autoindex"];
	else if (defaultServerDirectives.find("autoindex") != defaultServerDirectives.end())
		newLocation.root = defaultServerDirectives["autoindex"];
	
	if (locationDirectives.find("cgi_pass") != locationDirectives.end())
		newLocation.root = locationDirectives["cgi_pass"];
	else if (serverDirectives.find("cgi_pass") != serverDirectives.end())
		newLocation.root = serverDirectives["cgi_pass"];
	else if (defaultServerDirectives.find("cgi_pass") != defaultServerDirectives.end())
		newLocation.root = defaultServerDirectives["cgi_pass"];
	
	if (locationDirectives.find("client_max_body_size") != locationDirectives.end())
		newLocation.root = locationDirectives["client_max_body_size"];
	else if (serverDirectives.find("client_max_body_size") != serverDirectives.end())
		newLocation.root = serverDirectives["client_max_body_size"];
	else if (defaultServerDirectives.find("client_max_body_size") != defaultServerDirectives.end())
		newLocation.root = defaultServerDirectives["client_max_body_size"];
	
	// index

	// methods

	// redirect

	// error_page
}