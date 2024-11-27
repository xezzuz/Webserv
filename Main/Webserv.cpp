/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:42:01 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/27 20:05:32 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv(const std::string& configFileName) : WebservConfig(configFileName) {
	
}

Webserv::~Webserv() {
	
}

bool			Webserv::configurateWebserv() {
	if (!WebservConfig.openConfigFile())
		return false;
	if (!WebservConfig.parseConfigFile())
		return (std::cerr << BOLD << RED << "Webserv: " << WebservConfig.getConfigFileName() << " doesn't meet the requirements to run Webserv!" << RESET << std::endl, false);
	if (!WebservConfig.constructServers())
		return false;
	return true;
}

bool			Webserv::startWebserv() {
	std::cout << BOLD << "[WEBSERV]\tStarting the Webserv..." << RESET << std::endl;
	
	for (size_t i = 0; i < WebservConfig.getServers().size(); i++) {
		Servers.push_back(Server(WebservConfig.getServers()[i]));
		if (!Servers.back().getStatus()) {
			std::cout << "[WEBSERV]\tProblem occured during server" << i << " startup..." << std::endl;
			continue;
		}
		addToPoll(Servers[i].getServerSocket(), POLLIN, 0);
	}
	return true;
}

void			Webserv::addToPoll(int fd, short events, short revents) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
	toAdd.revents = revents;
	pollSockets.push_back(toAdd);
}

Config&			Webserv::getWebservConfig() {
	return this->WebservConfig;
}
