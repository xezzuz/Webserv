/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:06:37 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 20:43:00 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::vector<ServerConfig>& Configs) : vServerConfigs(Configs), status(false), serverSocket(-1) {
	
}

Server::Server(const Server& original) {
	*this = original;
}

Server&		Server::operator=(const Server& original) {
	if (this != &original) {
		this->port = original.port;
		this->status = original.status;
		this->serverSocket = original.serverSocket;
		this->vServerConfigs = original.vServerConfigs;
		this->Clients = original.Clients;
	}
	return *this;
}


Server::~Server() {
	// close(serverSocket);
}

bool		Server::initServer() {
	defaultConfig = vServerConfigs[0];
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "[SERVER]\tCreating a Socket failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		return (perror("socket"), false);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(defaultConfig.port);
	serverAddress.sin_addr.s_addr = parseIPv4(defaultConfig.host);
	
	int reUseAddr = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reUseAddr, sizeof(reUseAddr)); // check return value?
	
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "[SERVER]\tBinding failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		close(serverSocket);
		return (perror("bind"), false);
	}
	
	if (listen(serverSocket, 128) == -1) {
		std::cerr << "[SERVER]\tListening failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		close(serverSocket);
		return (perror("listen"), false);
	}
	
	status = true;
	std::cout << "[SERVER]\tListening on " << defaultConfig.host << ":" << defaultConfig.port << "..." << std::endl;
	return true;
}

bool		Server::handleEvent(pollfd& event, std::vector<pollfd>& pollSockets) {
	if (event.fd == serverSocket) {
		if (!handleServerSocketEvent(event, pollSockets)) {
			
		}
	} else {
		if (!handleClientSocketEvent(event, pollSockets)) {
			
		}
	}
	return true;
}

bool		Server::handleServerSocketEvent(pollfd&	pollServerSock, std::vector<pollfd>& pollSockets) {
	if (pollServerSock.revents & POLLIN) {
		int	newSocket = accept(serverSocket, NULL, NULL);
		if (newSocket == -1) {
			std::cerr << "[ERROR]\tAccepting failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			perror("accept");
			return false;
		}
		
		addToPoll(newSocket, POLLIN, 0, pollSockets);
		addToClientsMap(newSocket);

		std::cout << "[SERVER]\tAccepted Connection from Client "
				  << newSocket << "..." << std::endl;
	} else {
		std::cout << "Another event besides POLLIN" << std::endl;
	}
	// handle other revents
	return true;
}

bool		Server::handleClientSocketEvent(pollfd&	pollClientSock, std::vector<pollfd>& pollSockets) {
	int					clientSocket = pollClientSock.fd;
	if (pollClientSock.revents & POLLIN) {
		char				buffer[BUFFER_SIZE + 1];

		memset(buffer, 0, BUFFER_SIZE + 1);
		int	bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (bytesReceived > 0) {
			handleRequest(buffer, bytesReceived, clientSocket, pollClientSock);
		} else if (bytesReceived == 0) {
			std::cout << "[SERVER]\tClient " << clientSocket
					  << " disconnected..." << std::endl;
			rmFromClientsMap(clientSocket);
			rmFromPoll(clientSocket, pollSockets);
			return false;
		} else if (bytesReceived == -1) {
			std::cerr << "[ERROR]\tReceiving failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			// recv failed
			// close socket
			// remove from pollSockets
			rmFromPoll(clientSocket, pollSockets);
			perror("recv");
			return false;
		}
	} else if (pollClientSock.revents & POLLOUT) {
		std::cout << "Client socket ready to write!" << std::endl;
		Clients[clientSocket].getResponse().setvServerConfigs(vServerConfigs);
		Clients[clientSocket].getResponse().setResponsibleConfig();
		Clients[clientSocket].getResponse().feedResponse(&Clients[clientSocket].getRequest());
		if (Clients[clientSocket].getResponse().getResponseIsReady())
			send(clientSocket, Clients[clientSocket].getResponse().getRawResponse().c_str(), Clients[clientSocket].getResponse().getRawResponse().size(), 0);
		close(clientSocket);
	}
	// handle other events
	return true;
}

void		Server::handleRequest(char *buffer, int bufferSize, int clientSocket, pollfd& pollClientSock) {
	std::map<int, Client>::iterator	it = Clients.find(clientSocket);

	if (it == Clients.end()) {
		std::cout << "client was not found!" << std::endl;
		return ;
	}
	// could be optimized using iterators
	Clients[clientSocket].getRequest().feedRequest(buffer, bufferSize);
	Clients[clientSocket].getResponse().feedResponse(&Clients[clientSocket].getRequest());
	if (Clients[clientSocket].getRequest().getParsingState() == PARSING_FINISHED) {
		pollClientSock.events = POLLOUT;
		return ;
		// Clients[clientSocket].getResponse().setvServerConfigs(vServerConfigs);
		// Clients[clientSocket].getResponse().setResponsibleConfig();
	}
}

void		Server::addToPoll(int fd, short events, short revents, std::vector<pollfd>& pollSockets) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
	toAdd.revents = revents;
	pollSockets.push_back(toAdd);
}

void			Server::rmFromPoll(int fd, std::vector<pollfd>& pollSockets) {
	for (size_t i = 0; i < pollSockets.size(); i++) {
		if (pollSockets[i].fd == fd) {
			close(pollSockets[i].fd);
			pollSockets.erase(pollSockets.begin() + i);
			return ;
		}
	}
}

void		Server::addToClientsMap(int key) {
	Clients[key] = Client(key);
}

void		Server::rmFromClientsMap(int key) {
	std::map<int, Client>::iterator	it;

	it = Clients.find(key);
	if (it == Clients.end())
		return ;
	Clients.erase(it);
}
