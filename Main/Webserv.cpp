/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:42:01 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/30 16:31:24 by nazouz           ###   ########.fr       */
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
			Servers.pop_back();
			std::cout << "[WEBSERV]\tProblem occured during server " << i << " startup..." << std::endl;
			continue;
		}
		addToPoll(Servers.back().getServerSocket(), POLLIN, 0);
	}
	std::cout << "Webserv could run " << Servers.size() << " servers!" << std::endl;
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
			if (!pollSockets[i].revents)
				continue;
			Server*		respServer = getResponsibleServer(pollSockets[i].fd);
			
			if (!respServer) {
				std::cout << "no responsible server!" << std::endl;
				return false;
			}
			respServer->handleEvent(pollSockets[i], pollSockets);
		}
	}
	return true;
}

Server*			Webserv::getResponsibleServer(int eventSocket) {
	std::cout << "searching for responsible server of socket " << eventSocket << std::endl;
	for (size_t i = 0; i < Servers.size(); i++) {
		if (Servers[i].getServerSocket() == eventSocket)
			return &Servers[i];
		std::map<int, Client>::iterator		it = Servers[i].getClients().find(eventSocket);
		if (it != Servers[i].getClients().end())
			return &Servers[i];
	}
	return NULL;
}

void			Webserv::addToPoll(int fd, short events, short revents) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
	toAdd.revents = revents;
	pollSockets.push_back(toAdd);
}

void			Webserv::rmFromPoll(int fd) {
	for (size_t i = 0; i < pollSockets.size(); i++) {
		if (pollSockets[i].fd == fd) {
			close(pollSockets[i].fd);
			pollSockets.erase(pollSockets.begin() + i);
			return ;
		}
	}
}

void			Webserv::rmFromClientsMap(int key) {
	std::map<int, Client>::iterator		it;

	it = Clients.find(key);
	if (it == Clients.end()) {
		std::cout << "Client to remove was not found!" << std::endl;
		return ;
	}
	Clients.erase(it);
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
