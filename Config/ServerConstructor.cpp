/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConstructor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:55:44 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/01 19:33:53 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::constructServers() {
	for (size_t i = 0; i < serverBlocksIndexes.size(); i++) {
		int						start, end;
		ServerConfig			newServerBlock;
		
		start = serverBlocksIndexes[i].first;
		end = serverBlocksIndexes[i].second;
		if (!parseSingleServerBlock(start, end, newServerBlock))
			return false;
		Servers.push_back(newServerBlock);
	}
	return true;
}

bool				Config::parseSingleServerBlock(int start, int end, ServerConfig& currentServer) {
	std::vector<std::string>		alreadyParsed;
	
	for (int i = start + 1; i < end; i++) {
		if (configFileVector[i] == "[location]") {
			i = getBlockEndIndex(i, "[location]").second;
			continue;
		}

		size_t	equalsPos = configFileVector[i].find('=');
		if (equalsPos == std::string::npos)
			return (ErrorLogger("[SERVER] invalid line syntax : " + configFileVector[i]), false);
		std::string key = stringtrim(configFileVector[i].substr(0, equalsPos), " \t");
		std::string value = stringtrim(configFileVector[i].substr(equalsPos + 1), " \t");
		if (!fillServerBlockDirectives(key, value, alreadyParsed, currentServer))
			return false;
		alreadyParsed.push_back(key);
	}
	for (int i = start + 1; i < end; i++) {
		if (configFileVector[i] == "[location]") {
			if (!parseSingleLocationBlock(i, getBlockEndIndex(i, "[location]").second, currentServer))
				return false;
			i = getBlockEndIndex(i, "[location]").second;
		}
	}
	return true;
}

bool				Config::parseSingleLocationBlock(int start, int end, ServerConfig& currentServer) {
	Directives								LocationDirectives = currentServer.ServerDirectives;
	std::vector<std::string>				alreadyParsed;


	size_t equalsPos = configFileVector[start + 1].find("=");
	if (equalsPos == std::string::npos)
		return (ErrorLogger("[LOCATION] invalid line syntax : " + configFileVector[start + 1]), false);
	std::string location_key = stringtrim(configFileVector[start + 1].substr(0, equalsPos), " \t");
	std::string location_value = stringtrim(configFileVector[start + 1].substr(equalsPos + 1), " \t");
	if (location_key.empty() || location_value.empty())
		return (ErrorLogger("[LOCATION] invalid line syntax : " + location_key + " = " + location_value), false);
	if (location_key != "location")
		return (ErrorLogger("[LOCATION] location must be specified in the first line of location block"), false);
	if (!isValidLocation(location_value))
		return (ErrorLogger("[LOCATION] invalid line syntax : " + location_key + " = " + location_value), false);

	for (int i = start + 2; i < end; i++) {
		size_t	equalsPos = configFileVector[i].find('=');
		if (equalsPos == std::string::npos)
			return (ErrorLogger("[LOCATION] invalid line syntax : " + configFileVector[i]), false);
		std::string key = stringtrim(configFileVector[i].substr(0, equalsPos), " \t");
		std::string value = stringtrim(configFileVector[i].substr(equalsPos + 1), " \t");
		
		if (!fillLocationBlockDirectives(key, value, alreadyParsed, LocationDirectives))
			return false;
		alreadyParsed.push_back(key);
	}
	currentServer.Locations[location_value] = LocationDirectives;
	return true;
}

bool				Config::fillServerBlockDirectives(std::string& key, std::string& value, std::vector<std::string>& alreadyParsed, ServerConfig& currentServer) {
	if (key.empty() || value.empty())
		return (ErrorLogger("[SERVER] invalid line syntax : " + key + " = " + value), false);
	if (std::find(alreadyParsed.begin(), alreadyParsed.end(), key) != alreadyParsed.end() && key != "error_page" && key != "server_name")
		return (ErrorLogger("[SERVER] duplicate <" + key + "> directive  : " + key + " = " + value), false);
	
	std::vector<std::pair<std::string, bool (Config::*)(const std::string&, ServerConfig&)>>	limitedFunctions = {
		{"host", &Config::isValidHost},
		{"port", &Config::isValidPort},
		{"server_name", &Config::isValidServerName},
		
		// the following are only allowed in server block : host - port - server_name
	};

	std::vector<std::pair<std::string, bool (Config::*)(const std::string&, Directives&)>>	sharedFunctions = {
		{"error_page", &Config::isValidErrorPage},
		{"client_max_body_size", &Config::isValidClientMaxBodySize},
		{"root", &Config::isValidRoot},
		{"index", &Config::isValidIndex},
		{"auto_index", &Config::isValidAutoIndex},
		{"redirect", &Config::isValidRedirect},
		{"upload_store", &Config::isValidUploadStore},

		// the following are only allowed in location block : location - methods - alias - cgi_pass - cgi_ext
	};

	for (size_t i = 0; i < limitedFunctions.size(); i++) {
		if (limitedFunctions[i].first == key) {
			if (!(this->*(limitedFunctions[i].second))(value, currentServer))
				return (ErrorLogger("[SERVER] directive <" + key + "> is invalid : " + key + " = " + value), false);
			return true;
		}
	}

	for (size_t i = 0; i < sharedFunctions.size(); i++) {
		if (sharedFunctions[i].first == key) {
			if (!(this->*(sharedFunctions[i].second))(value, currentServer.ServerDirectives))
				return (ErrorLogger("[SERVER] directive <" + key + "> is invalid : " + key + " = " + value), false);
			return true;
		}
	}
	return (ErrorLogger("[SERVER] directive <" + key + "> is unknown or not allowed : " + key + " = " + value), false);
}

bool				Config::fillLocationBlockDirectives(std::string& key, std::string& value, std::vector<std::string>& alreadyParsed, Directives& toFill) {
	if (key.empty() || value.empty())
		return (ErrorLogger("[LOCATION] invalid line syntax : " + key + " = " + value), false);
	if (std::find(alreadyParsed.begin(), alreadyParsed.end(), key) != alreadyParsed.end())
		return (ErrorLogger("[LOCATION] duplicate <" + key + "> directive  : " + key + " = " + value), false);
	
	std::vector<std::pair<std::string, bool (Config::*)(const std::string&, Directives&)>>	sharedFunctions = {
		// the following only allowed in server block
		
		// {"host", &Config::isValidHost},
		// {"port", &Config::isValidPort},
		// {"server_name", &Config::isValidServerName},
		
		{"error_page", &Config::isValidErrorPage},
		{"client_max_body_size", &Config::isValidClientMaxBodySize},
		{"root", &Config::isValidRoot},
		{"index", &Config::isValidIndex},
		{"auto_index", &Config::isValidAutoIndex},
		{"redirect", &Config::isValidRedirect},
		{"upload_store", &Config::isValidUploadStore},
		// {"location", &Config::isValidLocation},
		{"methods", &Config::isValidMethods},
		{"alias", &Config::isValidAlias},
		{"cgi_ext", &Config::isValidCgiExt}
	};

	for (size_t i = 0; i < sharedFunctions.size(); i++) {
		if (sharedFunctions[i].first == key) {
			if (!(this->*(sharedFunctions[i].second))(value, toFill))
				return (ErrorLogger("[LOCATION] directive <" + key + "> is invalid : " + key + " = " + value), false);
			return true;
		}
	}
	return (ErrorLogger("[LOCATION] directive <" + key + "> is unknown or not allowed : " + key + " = " + value), false);
}
