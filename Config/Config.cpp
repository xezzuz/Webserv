/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:02 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 16:49:24 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>

Config::Config() {
	
}

Config::Config(const std::string& configFileName) {
	errorLog = open("../Logs/error.log", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (errorLog == -1)
		std::cerr << BOLD << "Webserv: can't create error.log!" << RESET << std::endl;

	if (!openConfigFile(configFileName))
		return ;
	
	fillDefaultDirectives();
}

Config::~Config() {
	if (configFile.is_open())
		configFile.close();
	close(errorLog);
}

void				Config::fillDefaultDirectives() {
	defaultServerDirectives["host"] = "0.0.0.0";
	defaultServerDirectives["port"] = "80";
	defaultServerDirectives["server_name"] = "none";
	defaultServerDirectives["error_page"] = "none";
	defaultServerDirectives["client_max_body_size"] = "none";
	defaultServerDirectives["root"] = "none";
	defaultServerDirectives["index"] = "index.html";
	defaultServerDirectives["methods"] = "GET POST DELETE";
	defaultServerDirectives["upload_store"] = "none";
	defaultServerDirectives["redirect"] = "none";
	defaultServerDirectives["autoindex"] = "off";
	defaultServerDirectives["cgi_pass"] = "none";

	defaultLocationDirectives["location"] = "none";
	defaultLocationDirectives["root"] = "none";
	defaultLocationDirectives["index"] = "index.html";
	defaultLocationDirectives["methods"] = "GET POST DELETE";
	defaultLocationDirectives["upload_store"] = "none";
	defaultLocationDirectives["redirect"] = "none";
	defaultLocationDirectives["autoindex"] = "off";
	defaultLocationDirectives["cgi_pass"] = "none";
	defaultLocationDirectives["error_page"] = "none";
	defaultLocationDirectives["client_max_body_size"] = "none";
}

void				Config::Logger(std::string error) {
	std::cerr << BOLD << RED << "Webserv: see error.log" << RESET << std::endl;
	error += "\n";
	write(errorLog, error.c_str(), error.length());
}
