/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Blocks.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 20:22:57 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/24 21:05:32 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::validateAllServerBlocks() {
	ServerConfig		newServer;

	for (size_t i = 0; i < serverBlocksIndexes.size(); i++) {
		int start, end;
		start = serverBlocksIndexes[i].first;
		end = serverBlocksIndexes[i].second;
		if (!validateSingleServerBlock(start, end, newServer))
			return false;
		servers.push_back(newServer);
	}
	return true;
}

bool				Config::validateSingleServerBlock(int start, int end, ServerConfig& currentServer) {
	std::map<std::string, std::string>		directives;
	std::string								allowedDirectives[5] = {"host", "port", "server_name", "error_page", "client_max_body_size"};
	for (size_t i = start + 1; i < end; i++) {
		// if it is a location block
		if (configFileVector[i] == "[location]") {
			if (!validateSingleLocationBlock(i, getBlockEndIndex(i, "[location]").second, currentServer))
				return false;
		}
		// if it is a directive
		size_t	equalsPos = configFileVector[i].find('=');
		if (equalsPos == std::string::npos)
			return false;
		std::string key = stringtrim(configFileVector[i].substr(0, equalsPos), " \t");
		std::string value = stringtrim(configFileVector[i].substr(equalsPos + 1), " \t");
		if (key.empty() || value.empty())
			return false;
		if (key != allowedDirectives[0] && key != allowedDirectives[1] 
			&& key != allowedDirectives[2] && key != allowedDirectives[3]
			&& key != allowedDirectives[4])
			return false;
		directives[key] = value;
	}
	if (!validateServerBlockDirectives(directives, currentServer)) {
		std::cout << "validateServerBlockDirectives(false)" << std::endl;
		return false;
	}
	std::cout << "validateSingleServerBlock(true)" << std::endl;
	return true;
}

bool				Config::validateSingleLocationBlock(int start, int end, ServerConfig& currentServer) {
	std::map<std::string, std::string>		directives;
	std::string								allowedDirectives[8] = {"location", "root", "index", "methods", "upload_store", "redirect", "autoindex", "cgi_pass"};
	for (size_t i = start + 1; i < end; i++) {
		size_t	equalsPos = configFileVector[i].find('=');
		if (equalsPos == std::string::npos)
			return false;
		std::string key = stringtrim(configFileVector[i].substr(0, equalsPos), " \t");
		std::string value = stringtrim(configFileVector[i].substr(equalsPos + 1), " \t");
		if (key.empty() || value.empty())
			return false;
		if (key != allowedDirectives[0] && key != allowedDirectives[1] 
			&& key != allowedDirectives[2] && key != allowedDirectives[3]
			&& key != allowedDirectives[4] && key != allowedDirectives[5]
			&& key != allowedDirectives[6] && key != allowedDirectives[7])
			return false;
		directives[key] = value;
	}
	if (!validateLocationBlockDirectives(directives, currentServer)) {
		std::cout << "validateLocationBlockDirectives(false)" << std::endl;
		return false;
	}
	std::cout << "validateSingleLocationBlock(true)" << std::endl;
	return true;
}

bool				Config::validateServerBlockDirectives(std::map<std::string, std::string>& directives, ServerConfig& currentServer) {
	if (directives.find("port") == directives.end())
		currentServer.port = 80;
	else if (isValidPort(directives["port"]))
		currentServer.port = std::atoi(directives["port"].c_str());
	else
		return (Logger("'port' directive is invalid in 'server block' :  \'" + directives["port"] + "\'"), false);
	
	
	if (directives.find("host") == directives.end())
		currentServer.host = "0.0.0.0";
	else if (isValidHost(directives["host"]))
		currentServer.host = directives["host"];
	else
		return (Logger("'host' directive is invalid in 'server block' :  \'" + directives["host"] + "\'"), false);
	
	
	if (directives.find("server_name") == directives.end())
		currentServer.server_name = "none";
	else if (isValidServerName(directives["server_name"]))
		currentServer.server_name = directives["server_name"];
	else
		return (Logger("'server_name' directive is invalid in 'server block' :  \'" + directives["server_name"] + "\'"), false);
	
	
	if (directives.find("error_page") == directives.end())
		currentServer.errorPage = "none";
	else if (isValidErrorPage(directives["error_page"]))
		currentServer.errorPage = directives["error_page"];
	else
		return (Logger("'error_page' directive is invalid in 'server block' :  \'" + directives["error_page"] + "\'"), false);

	
	if (directives.find("client_max_body_size") == directives.end())
		currentServer.clientMaxBodySize = 0; // default, need to be adjusted
	else if (isValidClientMaxBodySize(directives["client_max_body_size"])) // need to change its value to bytes
		currentServer.clientMaxBodySize = std::atoi(directives["client_max_body_size"].c_str());
	else
		return (Logger("'client_max_body_size' directive is invalid in 'server block' :  \'" + directives["client_max_body_size"] + "\'"), false);
	
	
	return true;
}

bool				Config::validateLocationBlockDirectives(std::map<std::string, std::string>& directives, ServerConfig& currentServer) {
	LocationConfig		newLocation;
	
	if (directives.find("location") == directives.end())
		return (Logger("'location' directive is missing in 'location block' : "), false);
	else if (isValidLocation(directives["location"]))
		newLocation.location = directives["location"];
	else
		return (Logger("'location' directive is invalid in 'location block' :  \'" + directives["location"] + "\'"), false);
	
	if (directives.find("root") == directives.end())
		return (Logger("'root' directive is missing in 'location block' : "), false);
	else if (isValidRoot(directives["root"]))
		newLocation.root = directives["root"];
	else
		return (Logger("'root' directive is invalid in 'location block' :  \'" + directives["root"] + "\'"), false);
	
	if (directives.find("index") == directives.end())
		newLocation.index = "index.html";
	else if (isValidIndex(directives["index"]))
		newLocation.index = directives["index"];
	else
		return (Logger("'index' directive is invalid in 'location block' :  \'" + directives["index"] + "\'"), false);
	
	if (directives.find("methods") == directives.end())
		newLocation.methods = "GET POST DELETE";
	else if (isValidMethods(directives["methods"]))
		newLocation.methods = directives["methods"];
	else
		return (Logger("'methods' directive is invalid in 'location block' :  \'" + directives["methods"] + "\'"), false);
	
	if (directives.find("upload_store") == directives.end())
		newLocation.upload_store = "/Users/nazouz/goinfre";
	else if (isValidUploadStore(directives["upload_store"]))
		newLocation.upload_store = directives["upload_store"];
	else
		return (Logger("'upload_store' directive is invalid in 'location block' :  \'" + directives["upload_store"] + "\'"), false);
	
	if (directives.find("redirect") == directives.end())
		newLocation.redirect = "none";
	else if (isValidRedirect(directives["redirect"]))
		newLocation.redirect = directives["redirect"];
	else
		return (Logger("'redirect' directive is invalid in 'location block' :  \'" + directives["redirect"] + "\'"), false);
	
	if (directives.find("autoindex") == directives.end())
		newLocation.autoindex = "off";
	else if (isValidAutoIndex(directives["autoindex"]))
		newLocation.autoindex = directives["autoindex"];
	else
		return (Logger("'autoindex' directive is invalid in 'location block' :  \'" + directives["autoindex"] + "\'"), false);
	
	if (directives.find("cgi_pass") == directives.end())
		newLocation.cgi_pass = "none";
	else if (isValidCgiPass(directives["cgi_pass"]))
		newLocation.cgi_pass = directives["cgi_pass"];
	else
		return (Logger("'cgi_pass' directive is invalid in 'location block' :  \'" + directives["cgi_pass"] + "\'"), false);
	
	currentServer.locations[newLocation.location] = newLocation;
	
	return true;
}
