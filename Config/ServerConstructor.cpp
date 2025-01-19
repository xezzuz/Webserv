/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConstructor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:55:44 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/19 18:02:44 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::constructServers() {
	for (size_t i = 0; i < Parser.size(); i++) {
		vServerConfig	newServerConfig;
		
		// Directives		newServerDirectives;
		fillServerDirectives(Parser[i].serverDirectives, newServerConfig.ServerDirectives);
		for (size_t j = 0; j < Parser[i].locationDirectives.size(); j++) {
			
			Directives	newLocationDirectives;
			fillLocationDirectives(Parser[i].locationDirectives[j], newLocationDirectives, newServerConfig.ServerDirectives);
			newServerConfig.Locations[Parser[i].locationDirectives[j]["location"]] = newLocationDirectives;
		}

		Servers.push_back(newServerConfig);
	}
	return true;
}

void				Config::setDefaultDirectives(std::map<std::string, std::string>& userDirectives) {
	std::map<std::string, std::string>		defaultDirectives;

	defaultDirectives["host"] = "0.0.0.0";
	defaultDirectives["port"] = "80";
	defaultDirectives["server_name"] = "none";
	defaultDirectives["error_page"] = "none";
	defaultDirectives["client_max_body_size"] = "none";
	defaultDirectives["root"] = "/home/mmaila/Desktop/SERV/www";
	defaultDirectives["alias"] = "none";
	defaultDirectives["index"] = "index.html";
	defaultDirectives["methods"] = "GET POST DELETE";
	defaultDirectives["upload_store"] = "/home/nazouz/goinfre/";
	defaultDirectives["autoindex"] = "off";
	defaultDirectives["redirect"] = "none";
	defaultDirectives["cgi_pass"] = "none";
	defaultDirectives["cgi_ext"] = "none";

	// check here
	std::map<std::string, std::string>::iterator	it;
	for (; it != defaultDirectives.end(); it++) {
		if (userDirectives.find(it->first) == userDirectives.end())
			userDirectives[it->first] = defaultDirectives[it->first];
	}
}

bool				Config::fillServerDirectives(std::map<std::string, std::string>& ServerParserDirectives, Directives& ServerDirectives) {
	setDefaultDirectives(ServerParserDirectives);
	
	ServerDirectives.host = ServerParserDirectives["host"];
	ServerDirectives.port = std::atoi(ServerParserDirectives["port"].c_str());
	ServerDirectives.server_names = splitStringBySpace(ServerParserDirectives["server_name"]);
	// ServerDirectives.error_pages = parseErrorPages();
	// ServerDirectives.client_max_body_size = parseClientMaxBodySize();
	ServerDirectives.root = ServerParserDirectives["root"];
	ServerDirectives.alias = ServerParserDirectives["alias"];
	ServerDirectives.index = splitStringBySpace(ServerParserDirectives["index"]);
	ServerDirectives.methods = splitStringBySpace(ServerParserDirectives["methods"]);
	ServerDirectives.upload_store = ServerParserDirectives["upload_store"];
	// ServerDirectives.autoindex = parseAutoIndex();
	ServerDirectives.redirect = splitStringBySpace(ServerParserDirectives["redirect"]);
	ServerDirectives.cgi_pass = ServerParserDirectives["cgi_pass"];
	ServerDirectives.cgi_ext = ServerParserDirectives["cgi_ext"];











	
	// std::string			temp_server_name;
	// if (serverDirectives.find("server_name") != serverDirectives.end())
	// 	temp_server_name = serverDirectives["server_name"];
	// else
	// 	temp_server_name = defaultDirectives["server_name"];

	// std::string			token;
	// std::stringstream	ss(temp_server_name);
	// while (ss >> token)
	// 	Server.server_name.push_back(token);
	
	return true;
}

bool				Config::fillLocationDirectives(std::map<std::string, std::string>& ServerParserDirectives, Directives& LocationDirectives, Directives& ServerDirectives) {
	// setting location directives to server block directives
	LocationDirectives.port = ServerDirectives.port;
	LocationDirectives.host = ServerDirectives.host;
	LocationDirectives.server_names = ServerDirectives.server_names;
	LocationDirectives.error_pages = ServerDirectives.error_pages;
	LocationDirectives.client_max_body_size = ServerDirectives.client_max_body_size;
	LocationDirectives.root = ServerDirectives.root;
	LocationDirectives.alias = ServerDirectives.alias;
	LocationDirectives.index = ServerDirectives.index;
	LocationDirectives.methods = ServerDirectives.methods;
	LocationDirectives.upload_store = ServerDirectives.upload_store;
	LocationDirectives.autoindex = ServerDirectives.autoindex;
	LocationDirectives.redirect = ServerDirectives.redirect;
	LocationDirectives.cgi_pass = ServerDirectives.cgi_pass;
	LocationDirectives.cgi_ext = ServerDirectives.cgi_ext;


	// overriding location directives with location block directives
	LocationDirectives.host = ServerParserDirectives["host"];
	LocationDirectives.port = std::atoi(ServerParserDirectives["port"].c_str());
	LocationDirectives.server_names = splitStringBySpace(ServerParserDirectives["server_name"]);
	// LocationDirectives.error_pages = parseErrorPages();
	// LocationDirectives.client_max_body_size = parseClientMaxBodySize();
	LocationDirectives.root = ServerParserDirectives["root"];
	LocationDirectives.alias = ServerParserDirectives["alias"];
	LocationDirectives.index = splitStringBySpace(ServerParserDirectives["index"]);
	LocationDirectives.methods = splitStringBySpace(ServerParserDirectives["methods"]);
	LocationDirectives.upload_store = ServerParserDirectives["upload_store"];
	// LocationDirectives.autoindex = parseAutoIndex();
	LocationDirectives.redirect = splitStringBySpace(ServerParserDirectives["redirect"]);
	LocationDirectives.cgi_pass = ServerParserDirectives["cgi_pass"];
	LocationDirectives.cgi_ext = ServerParserDirectives["cgi_ext"];
	return true;
}
























// bool				Config::fillLocationConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, std::map<std::string, std::string>& locationDirectives, vServerConfig& Server) {
// 	LocationConfig			newLocation;
	
// 	newLocation.location = locationDirectives["location"];
	
// 	if (locationDirectives.find("root") != locationDirectives.end())
// 		newLocation.root = locationDirectives["root"];
// 	else if (serverDirectives.find("root") != serverDirectives.end())
// 		newLocation.root = serverDirectives["root"];
// 	else if (defaultServerDirectives.find("root") != defaultServerDirectives.end())
// 		newLocation.root = defaultServerDirectives["root"];
	
// 	if (locationDirectives.find("upload_store") != locationDirectives.end())
// 		newLocation.upload_store = locationDirectives["upload_store"];
// 	else if (serverDirectives.find("upload_store") != serverDirectives.end())
// 		newLocation.upload_store = serverDirectives["upload_store"];
// 	else if (defaultServerDirectives.find("upload_store") != defaultServerDirectives.end())
// 		newLocation.upload_store = defaultServerDirectives["upload_store"];
	
// 	if (locationDirectives.find("autoindex") != locationDirectives.end())
// 		newLocation.autoindex = locationDirectives["autoindex"];
// 	else if (serverDirectives.find("autoindex") != serverDirectives.end())
// 		newLocation.autoindex = serverDirectives["autoindex"];
// 	else if (defaultServerDirectives.find("autoindex") != defaultServerDirectives.end())
// 		newLocation.autoindex = defaultServerDirectives["autoindex"];
	
// 	if (locationDirectives.find("cgi_pass") != locationDirectives.end())
// 		newLocation.cgi_pass = locationDirectives["cgi_pass"];
// 	else if (serverDirectives.find("cgi_pass") != serverDirectives.end())
// 		newLocation.cgi_pass = serverDirectives["cgi_pass"];
// 	else if (defaultServerDirectives.find("cgi_pass") != defaultServerDirectives.end())
// 		newLocation.cgi_pass = defaultServerDirectives["cgi_pass"];
	
// 	if (locationDirectives.find("client_max_body_size") != locationDirectives.end())
// 		newLocation.client_max_body_size = locationDirectives["client_max_body_size"];
// 	else if (serverDirectives.find("client_max_body_size") != serverDirectives.end())
// 		newLocation.client_max_body_size = serverDirectives["client_max_body_size"];
// 	else if (defaultServerDirectives.find("client_max_body_size") != defaultServerDirectives.end())
// 		newLocation.client_max_body_size = defaultServerDirectives["client_max_body_size"];
	
// 	// index
// 	if (locationDirectives.find("index") == locationDirectives.end())
// 		newLocation.index.push_back(defaultLocationDirectives["index"]);
// 	else if (locationDirectives.find("index") != locationDirectives.end()) {
// 		std::string			token;
// 		std::stringstream	ss(locationDirectives["index"]);
// 		while (ss >> token)
// 			newLocation.index.push_back(token);
// 	}
	
// 	// methods
// 	if (locationDirectives.find("methods") == locationDirectives.end())
// 		newLocation.methods.push_back(defaultLocationDirectives["methods"]);
// 	else if (locationDirectives.find("methods") != locationDirectives.end()) {
// 		std::string			token;
// 		std::stringstream	ss(locationDirectives["methods"]);
// 		while (ss >> token)
// 			newLocation.methods.push_back(token);
// 	}
	
// 	// redirect
// 	if (locationDirectives.find("redirect") == locationDirectives.end())
// 		newLocation.redirect.push_back(defaultLocationDirectives["redirect"]);
// 	else if (locationDirectives.find("redirect") != locationDirectives.end()) {
// 		std::string			token;
// 		std::stringstream	ss(locationDirectives["redirect"]);
// 		while (ss >> token)
// 			newLocation.redirect.push_back(token);
// 	}
	
// 	// error_page
// 	if (locationDirectives.find("error_page") == locationDirectives.end())
// 		newLocation.error_page.push_back(defaultLocationDirectives["error_page"]);
// 	else if (locationDirectives.find("error_page") != locationDirectives.end()) {
// 		std::string			token;
// 		std::stringstream	ss(locationDirectives["error_page"]);
// 		while (ss >> token)
// 			newLocation.error_page.push_back(token);
// 	}

// 	Server.locations.push_back(newLocation);
// 	return true;
// }
