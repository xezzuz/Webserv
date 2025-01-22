/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:42:01 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/22 15:44:20 by mmaila           ###   ########.fr       */
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
	
	std::map<std::string, std::vector<vServerConfig>>		vServersConfigs;
	std::vector<vServerConfig>								allConfigs = WebservConfig.getServers();
	
	for (size_t i = 0; i < allConfigs.size(); i++) {
		std::stringstream	ss;
		ss << allConfigs[i].port;
		std::string	vs_port = ss.str();
		std::string	vs_host = allConfigs[i].host;

		std::map<std::string, std::vector<vServerConfig>>::iterator it = vServersConfigs.find(vs_host + ":" + vs_port);
		if (it == vServersConfigs.end()) {
			std::vector<vServerConfig>	temp(1, allConfigs[i]);
			vServersConfigs[vs_host + ":" + vs_port] = temp;
		} else if (it != vServersConfigs.end()) {
			it->second.push_back(allConfigs[i]);
		}
	}
	
	std::map< std::string, std::vector<vServerConfig> >::iterator it;
	for (it = vServersConfigs.begin(); it != vServersConfigs.end(); it++) {
		// std::cout << it->first << " has " << it->second.size() << " configs" << std::endl;

		vServers.push_back(Server(it->second));
		vServers.back().initServer();
		addToPoll(vServers.back().getServerSocket(), POLLIN);
	}
	
	// std::cout << "Webserv could run " << vServers.size() << " vServers!" << std::endl;
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
				return false; // should not return false / should delete the fd from poll list
			}
			respServer->handleEvent(pollSockets[i], pollSockets);
		}
	}
	return true;
}

Server*			Webserv::getResponsibleServer(int eventSocket) {
	// std::cout << "searching for responsible server of socket " << eventSocket << std::endl;
	for (size_t i = 0; i < vServers.size(); i++) {
		if (vServers[i].getServerSocket() == eventSocket)
			return &vServers[i];
		std::map<int, Client>::iterator		it = vServers[i].getClients().find(eventSocket);
		if (it != vServers[i].getClients().end())
			return &vServers[i];
	}
	return NULL;
}

void			Webserv::addToPoll(int fd, short events) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
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

// void			Webserv::rmFromClientsMap(int key) {
// 	std::map<int, Client>::iterator		it;

// 	it = Clients.find(key);
// 	if (it == Clients.end()) {
// 		std::cout << "Client to remove was not found!" << std::endl;
// 		return ;
// 	}
// 	Clients.erase(it);
// }

bool			Webserv::isServerSocket(const int socket) {
	for (size_t i = 0; i < vServers.size(); i++) {
		if (vServers[i].getServerSocket() == socket)
			return true;
	}
	return false;
}

Config&			Webserv::getWebservConfig() {
	return this->WebservConfig;
}
