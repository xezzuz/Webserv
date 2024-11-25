/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConstructor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:55:44 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 19:55:06 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::constructServers() {
	for (size_t i = 0; i < Parser.size(); i++) {
		ServerConfig		newServer;

		fillServerConfigByParserDirectives(Parser[i].serverDirectives, newServer);
		for (size_t j = 0; Parser[i].locationDirectives.size(); j++) {
			LocationConfig		newLocation;
			fillLocationConfigByParserDirectives(Parser[i].locationDirectives[j], newServer);
		}
	}
	return true;
}

bool				Config::fillServerConfigByParserDirectives(std::map<std::string, std::string>& directives, ServerConfig& Server) {

	if (directives.find("port") != directives.end())
		Server.port = std::atoi(directives["port"].c_str());
	else
		Server.port = std::atoi(defaultServerDirectives["port"].c_str());
	
	if (directives.find("host") != directives.end())
		Server.host = directives["host"];
	else
		Server.host = defaultServerDirectives["host"];
	
	std::string			temp_server_name;
	if (directives.find("server_name") != directives.end())
		temp_server_name = directives["server_name"];
	else
		temp_server_name = defaultServerDirectives["server_name"];
	
	std::string			token;
	std::stringstream	ss(temp_server_name);
	while (ss >> token)
		Server.server_name.push_back(token);
	
	return true;
}

bool				Config::fillLocationConfigByParserDirectives(std::map<std::string, std::string>& directives, ServerConfig& Server) {
	
}