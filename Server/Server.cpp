/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:06:37 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/30 17:26:19 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerConfig& config) : config(config) {
	status = false;
	serverSocket = -1;
	// printf("Server::Consctructor | %p\n", this);
}

Server::Server(const Server& original) {
	*this = original;
}

Server&		Server::operator=(const Server& original) {
	if (this != &original) {
		this->port = original.port;
		this->status = original.status;
		this->serverSocket = original.serverSocket;
		this->config = original.config;
		this->Clients = original.Clients;
	}
	return *this;
}


Server::~Server() {
	// printf("Server::Desctructor | %p\n", this);
	// close(serverSocket);
}

void		Server::startWebserv() {
	// acceptConnections();
}

void		Server::stopWebserv() {
	
}

bool		Server::initServer() {
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "[SERVER]\tCreating a Socket failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		return (perror("socket"), false);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(config.port);
	serverAddress.sin_addr.s_addr = parseIPv4(config.host);
	
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
	std::cout << "[SERVER]\tListening on " << config.host << ":" << config.port << "..." << std::endl;
	return true;
}

bool		Server::handleEvent(pollfd& event, std::vector<pollfd>& pollSockets) {
	std::cout << "This is server " << config.host << ":" << config.port << std::endl;
	if (event.fd == serverSocket) {
		std::cout << "Server socket " << config.port << std::endl;
		if (!handleServerSocketEvent(event, pollSockets)) {
			
		}
	} else {
		std::cout << "Client socket " << config.port << std::endl;
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
	if (pollClientSock.revents & POLLIN) {
		char				buffer[BUFFER_SIZE + 1];
		int					clientSocket = pollClientSock.fd;

		memset(buffer, 0, BUFFER_SIZE + 1);
		int	bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (bytesReceived > 0) {
			handleRequest(buffer, bytesReceived, clientSocket);
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
	} else {
		std::cout << "Another event besides POLLIN" << std::endl;
	}
	// handle other events
	return true;
}

void		Server::handleRequest(char *buffer, int bufferSize, int clientSocket) {
	std::map<int, Client>::iterator	it = Clients.find(clientSocket);

	// could be optimized using iterators
	if (it == Clients.end()) {
		// new client
		Clients[clientSocket] = Client(clientSocket);
		Clients[clientSocket].getRequest().feedRequest(buffer, bufferSize);
	} else {
		// old client
		if (Clients[clientSocket].getRequest().getParsingState() == PARSING_FINISHED)
			return ;
		Clients[clientSocket].getRequest().feedRequest(buffer, bufferSize);
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
