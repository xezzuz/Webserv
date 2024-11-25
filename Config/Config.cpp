/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:02 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 12:45:00 by nazouz           ###   ########.fr       */
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
	
	fillDefaultDirectives();

	if (!parseConfigFile()) {
		std::cerr << RED << "Config File is not valid..." << std::endl;
		return ;
	}
	std::cerr << GREEN << "Config File is valid..." << std::endl;
}

Config::~Config() {
	
}

void				Config::fillDefaultDirectives() {
	defaultServerDirectives["host"] = "0.0.0.0";
	defaultServerDirectives["port"] = "80";
	defaultServerDirectives["server_name"] = "none";
	defaultServerDirectives["error_page"] = "none";
	defaultServerDirectives["client_max_body_size"] = "none";

	defaultLocationDirectives["location"] = "none";
	defaultLocationDirectives["root"] = "none";
	defaultLocationDirectives["index"] = "index.html";
	defaultLocationDirectives["methods"] = "GET POST DELETE";
	defaultLocationDirectives["upload_store"] = "none";
	defaultLocationDirectives["redirect"] = "none";
	defaultLocationDirectives["autoindex"] = "off";
	defaultLocationDirectives["cgi_pass"] = "none";
}

void				Config::Logger(const std::string& error) {
	std::cerr << RED << error << RESET << std::endl;
}
