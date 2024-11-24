/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:02 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/24 19:40:27 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>

Config::Config() {
	
}

Config::Config(const std::string& configFileName) {
	if (!openConfigFile(configFileName)) {
		std::cerr << "Error Opening Config File..." << std::endl;
		return ;
	}
	
	if (!parseConfigFile()) {
		std::cerr << RED << "Config File is not valid..." << std::endl;
		return ;
	}
	std::cerr << GREEN << "Config File is valid..." << std::endl;
}

Config::~Config() {
	
}

bool				Config::openConfigFile(const std::string& configFileName) {
	int					pos = configFileName.find(".conf");
	if (pos == 0 || pos == std::string::npos || configFileName.substr(pos) != ".conf")
		return false;
	
	configFile.open(configFileName, std::ios::in);
	if (!configFile.is_open())
		return false;
	return true;
}

bool				Config::parseConfigFile() {
	storeConfigFileInVector();
	if (!basicBlocksCountCheck())
		return false;
	fillServerBlocksIndexes();
	if (!validateBlocksIndexes()) {
		std::cout << "[BLOCKS MISPLACEMENT]\n";
		return false;
	}
	if (!validateAllServerBlocks()) {
		std::cout << "[SERVER BLOCK]\n";
		return false;
	}
	return true;
}

bool				Config::storeConfigFileInVector() {
	std::string			line;

	while (std::getline(configFile, line)) {
		if (line.empty())
			continue;
		if (stringtrim(line, " \t").empty())
			continue;
		if (stringtrim(line, " \t")[0] == '#')
			continue;
		configFileVector.push_back(line);
		if (configFile.eof())
			break;
	}
	return true;
}

bool				Config::basicBlocksCountCheck() {
	int					serverStart = 0;
	int					serverEnd = 0;
	int					locationStart = 0;
	int					locationEnd = 0;
	
	for (size_t i = 0; i < configFileVector.size(); i++) {
		if (configFileVector[i] == "[server]")
			serverStart++;
		else if (configFileVector[i] == "[/server]")
			serverEnd++;
		else if (configFileVector[i] == "[location]")
			locationStart++;
		else if (configFileVector[i] == "[/location]")
			locationEnd++;
	}
	std::cout << "[server] = " << serverStart << " [/server] = " << serverEnd << std::endl;
	std::cout << "[location] = " << locationStart << " [/location] = " << locationEnd << std::endl;
	if (!serverStart || !serverEnd)
		return false;
	if ((serverStart != serverEnd) || (locationStart != locationEnd))
		return false;
	std::cout << "basicBlocksCountCheck(true)" << std::endl;
	return true;
}

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

bool				Config::validateServerBlockDirectives(std::map<std::string, std::string>& directives, ServerConfig& currentServer) {
	if (directives.find("port") == directives.end())
		currentServer.port = 80;
	else if (isValidPort(directives["port"]))
		currentServer.port = std::atoi(directives["port"].c_str());
	else
		return false;
	
	
	if (directives.find("host") == directives.end())
		currentServer.host = "0.0.0.0";
	else if (isValidHost(directives["host"]))
		currentServer.host = directives["host"];
	else
		return false;
	
	
	if (directives.find("server_name") == directives.end())
		currentServer.server_name = "";
	else if (isValidServerName(directives["server_name"]))
		currentServer.server_name = directives["server_name"];
	else
		return false;
	
	
	if (directives.find("error_page") == directives.end())
		currentServer.errorPage = "";
	else if (isValidErrorPage(directives["error_page"]))
		currentServer.errorPage = directives["error_page"];
	else
		return false;

	// if (directives.find("client_max_body_size") == directives.end())
	// 	currentServer.clientMaxBodySize = "";
	// else if (!isValidErrorPage(directives["client_max_body_size"]))
	// 	return false;
	// if (directives.find("client_max_body_size") != directives.end())
	// 	currentServer.clientMaxBodySize = directives["client_max_body_size"];
	return true;
}

bool				Config::validateSingleServerBlock(int start, int end, ServerConfig& currentServer) {
	std::map<std::string, std::string>		directives;
	std::string								allowedDirectives[5] = {"host", "port", "server_name", "error_page", "client_max_body_size"};
	for (size_t i = start + 1; i < end; i++) {
		// if it is a location block
		if (configFileVector[i] == "[location]") {
			if (!validateSingleLocationBlock(i, getBlockEndIndex(i, "[location]").first, currentServer))
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
	if (!validateServerBlockDirectives(directives, currentServer))
		return false;
	std::cout << "validateSingleServerBlock(true)" << std::endl;
	return true;
}

bool				Config::validateLocationBlockDirectives(std::map<std::string, std::string>& directives, ServerConfig& currentServer) {
	LocationConfig		newLocation;
	
	if (directives.find("location") == directives.end())
		return false;
	else if (isValidLocation(directives["location"]))
		newLocation.location = directives["location"];
	else
		return false;
	
	if (directives.find("root") == directives.end())
		return false;
	else if (isValidRoot(directives["root"]))
		newLocation.root = directives["root"];
	else
		return false;
	
	if (directives.find("index") == directives.end())
		newLocation.index = "default";
	else if (isValidIndex(directives["index"]))
		newLocation.index = directives["index"];
	else
		return false;
	
	if (directives.find("methods") == directives.end())
		newLocation.methods = "default";
	else if (isValidMethods(directives["methods"]))
		newLocation.methods = directives["methods"];
	else
		return false;
	
	if (directives.find("upload_store") == directives.end())
		newLocation.upload_store = "default";
	else if (isValidUploadStore(directives["upload_store"]))
		newLocation.upload_store = directives["upload_store"];
	else
		return false;
	
	if (directives.find("redirect") == directives.end())
		newLocation.redirect = "default";
	else if (isValidRedirect(directives["redirect"]))
		newLocation.redirect = directives["redirect"];
	else
		return false;
	
	if (directives.find("autoindex") == directives.end())
		newLocation.autoindex = "default";
	else if (isValidAutoIndex(directives["autoindex"]))
		newLocation.autoindex = directives["autoindex"];
	else
		return false;
	
	if (directives.find("cgi_pass") == directives.end())
		newLocation.cgi_pass = "default";
	else if (isValidCgiPass(directives["cgi_pass"]))
		newLocation.cgi_pass = directives["cgi_pass"];
	else
		return false;
	
	currentServer.locations[newLocation.location] = newLocation;
	
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
	if (!validateLocationBlockDirectives(directives, currentServer))
		return false;
	std::cout << "validateSingleLocationBlock(true)" << std::endl;
	return true;
}

bool				Config::locationBlockIsInsideAServerBlock(int locationStart, int locationEnd) {
	// std::pair<int, int>		matchingServerBlock;
	for (size_t i = 0; i < serverBlocksIndexes.size(); i++) {
		if (serverBlocksIndexes[i].first < locationStart && serverBlocksIndexes[i].second > locationEnd)
			return true;
	}
	return false;
}

bool				Config::validateBlocksIndexes() {
	int			a, b, c, d;
	printf("SERVER BLOCKS:\n");
	for (size_t i = 0; i < serverBlocksIndexes.size() - 1; i++) {
		a = serverBlocksIndexes[i].first;
		b = serverBlocksIndexes[i].second;
		c = serverBlocksIndexes[i + 1].first;
		d = serverBlocksIndexes[i + 1].second;
		printf("[%d][%d] [%d][%d]\n", a, b, c, d);
		if (a == -1 || b == -1 || c == -1 || d == -1)
			return false;
		if (a < b && b < c && c < d)
			continue;
		return false;
	}
	if (!locationBlocksIndexes.size()) {
		std::cout << "validateBlocksIndexes(true)" << std::endl;
		return true;
	}
	printf("LOCATION BLOCKS:\n");
	for (size_t i = 0; i < static_cast<int>(locationBlocksIndexes.size() - 1); i++) {
		a = locationBlocksIndexes[i].first;
		b = locationBlocksIndexes[i].second;
		c = locationBlocksIndexes[i + 1].first;
		d = locationBlocksIndexes[i + 1].second;
		printf("[%d][%d] [%d][%d]\n", a, b, c, d);
		if (a == -1 || b == -1 || c == -1 || d == -1)
			return false;
		if (!(a < b && b < c && c < d))
			return false;
		if (locationBlockIsInsideAServerBlock(a, b))
			continue;
		return false;
	}
	a = locationBlocksIndexes[locationBlocksIndexes.size() - 1].first;
	b = locationBlocksIndexes[locationBlocksIndexes.size() - 1].second;
	if (!locationBlockIsInsideAServerBlock(a, b)) {
		printf("here\n");
		return false;
	}
	std::cout << "validateBlocksIndexes(true)" << std::endl;
	return true;
}

std::pair<int, int>		Config::getBlockEndIndex(int blockStart, const std::string startBlockStr) {
	int						i = blockStart;
	std::string				endBlock;
	std::pair<int, int>		toReturn(blockStart, -1);
	
	if (startBlockStr == "[server]")
		endBlock = "[/server]";
	else if (startBlockStr == "[location]")
		endBlock = "[/location]";

	for (; i < configFileVector.size(); i++) {
		if (configFileVector[i] == endBlock) {
			toReturn = std::make_pair(blockStart, i);
			return toReturn;
		}
	}
	return toReturn;
}

void				Config::fillServerBlocksIndexes() {
	for (size_t i = 0; i < configFileVector.size(); i++) {
		if (configFileVector[i] == "[server]")
			serverBlocksIndexes.push_back(getBlockEndIndex(i, "[server]"));
		else if (configFileVector[i] == "[location]")
			locationBlocksIndexes.push_back(getBlockEndIndex(i, "[location]"));
	}
}
