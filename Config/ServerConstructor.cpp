/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConstructor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:55:44 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/17 18:54:49 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::constructServers() {
	for (size_t i = 0; i < Parser.size(); i++) {
		ServerConfig		newServer;

		fillServerConfigByParserDirectives(Parser[i].serverDirectives, newServer);
		for (size_t j = 0; j < Parser[i].locationDirectives.size(); j++) {
			fillLocationConfigByParserDirectives(Parser[i].serverDirectives, Parser[i].locationDirectives[j], newServer);
		}
		Servers.push_back(newServer);
	}
	return true;
}

bool				Config::fillServerConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, ServerConfig& Server) {

    std::cout << serverDirectives["error_page"] << std::endl;
    

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
		newLocation.upload_store = locationDirectives["upload_store"];
	else if (serverDirectives.find("upload_store") != serverDirectives.end())
		newLocation.upload_store = serverDirectives["upload_store"];
	else if (defaultServerDirectives.find("upload_store") != defaultServerDirectives.end())
		newLocation.upload_store = defaultServerDirectives["upload_store"];
	
	if (locationDirectives.find("autoindex") != locationDirectives.end())
		newLocation.autoindex = locationDirectives["autoindex"];
	else if (serverDirectives.find("autoindex") != serverDirectives.end())
		newLocation.autoindex = serverDirectives["autoindex"];
	else if (defaultServerDirectives.find("autoindex") != defaultServerDirectives.end())
		newLocation.autoindex = defaultServerDirectives["autoindex"];
	
	if (locationDirectives.find("cgi_pass") != locationDirectives.end())
		newLocation.cgi_pass = locationDirectives["cgi_pass"];
	else if (serverDirectives.find("cgi_pass") != serverDirectives.end())
		newLocation.cgi_pass = serverDirectives["cgi_pass"];
	else if (defaultServerDirectives.find("cgi_pass") != defaultServerDirectives.end())
		newLocation.cgi_pass = defaultServerDirectives["cgi_pass"];
	
	if (locationDirectives.find("client_max_body_size") != locationDirectives.end())
		newLocation.client_max_body_size = locationDirectives["client_max_body_size"];
	else if (serverDirectives.find("client_max_body_size") != serverDirectives.end())
		newLocation.client_max_body_size = serverDirectives["client_max_body_size"];
	else if (defaultServerDirectives.find("client_max_body_size") != defaultServerDirectives.end())
		newLocation.client_max_body_size = defaultServerDirectives["client_max_body_size"];
	
	// index
	if (locationDirectives.find("index") == locationDirectives.end())
		newLocation.index.push_back(defaultLocationDirectives["index"]);
	else if (locationDirectives.find("index") != locationDirectives.end()) {
		std::string			token;
		std::stringstream	ss(locationDirectives["index"]);
		while (ss >> token)
			newLocation.index.push_back(token);
	}
	
	// methods
	if (locationDirectives.find("methods") == locationDirectives.end())
		newLocation.methods.push_back(defaultLocationDirectives["methods"]);
	else if (locationDirectives.find("methods") != locationDirectives.end()) {
		std::string			token;
		std::stringstream	ss(locationDirectives["methods"]);
		while (ss >> token)
			newLocation.methods.push_back(token);
	}
	
	// redirect
	if (locationDirectives.find("redirect") == locationDirectives.end())
		newLocation.redirect.push_back(defaultLocationDirectives["redirect"]);
	else if (locationDirectives.find("redirect") != locationDirectives.end()) {
		std::string			token;
		std::stringstream	ss(locationDirectives["redirect"]);
		while (ss >> token)
			newLocation.redirect.push_back(token);
	}
	
	// error_page
	if (locationDirectives.find("error_page") == locationDirectives.end())
		newLocation.error_page.push_back(defaultLocationDirectives["error_page"]);
	else if (locationDirectives.find("error_page") != locationDirectives.end()) {
		std::string			token;
		std::stringstream	ss(locationDirectives["error_page"]);
		while (ss >> token)
			newLocation.error_page.push_back(token);
	}

	Server.locations.push_back(newLocation);
	return true;
}
