/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:42:01 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/28 14:00:40 by nazouz           ###   ########.fr       */
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
		Servers.back().initServer();
		if (!Servers.back().getStatus()) {
			std::cout << "[WEBSERV]\tProblem occured during server" << i << " startup..." << std::endl;
			continue;
		}
		addToPoll(Servers[i].getServerSocket(), POLLIN, 0);
	}
	return true;
}

bool			Webserv::monitorWebserv() {
	while (true) {
		int		pollReturn = poll(&pollSockets[0], pollSockets.size(), POLL_BLOCK);
		if (pollReturn == -1) {
			std::cerr << "[ERROR]\tPolling failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			// close all sockets?
			return (perror("poll"), false);
		}

		for (size_t i = 0; i < pollSockets.size(); i++) {
			if (isServerSocket(pollSockets[i].fd)) {
				if (!handleServerSocketEvents(pollSockets[i]))
					continue;
			} else {
				if (!handleClientSocketEvents(pollSockets[i]))
					i--;
			}
		}
	}
}

bool			Webserv::handleServerSocketEvents(pollfd&	serverPollfd) {
	
}

bool			Webserv::handleClientSocketEvents(pollfd&	clientPollfd) {
	
}

void			Webserv::addToPoll(int fd, short events, short revents) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
	toAdd.revents = revents;
	pollSockets.push_back(toAdd);
}

bool			Webserv::isServerSocket(const int socket) {
	for (size_t i = 0; i < Servers.size(); i++) {
		if (Servers[i].getServerSocket() == socket)
			return true;
	}
	return false;
}

Config&			Webserv::getWebservConfig() {
	return this->WebservConfig;
}
