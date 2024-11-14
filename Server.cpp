/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:06:37 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/14 21:53:41 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const int _port) : port(_port) {
	status = initServer();
}

Server::~Server() {
	close(serverSocket);
}

void		Server::startWebserv() {

}

void		Server::stopWebserv() {

}

bool		Server::initServer() {
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		return (perror("socket"), false);
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "[ERROR]\tBinding failed..." << std::endl;
		std::cerr << "[ERROR]\t";
		// close(serverSocket);
		return (perror("bind"), false);
	}
	
	if (listen(serverSocket, 128)) {
		std::cerr << "[ERROR]\tListening failed..." << std::endl;
		std::cerr << "[ERROR]\t";
		// close(serverSocket);
		return (perror("listen"), false);
	}
	
	std::cout << "[SERVER]\tWebserv is listening on port " << port << "..." << std::endl;
	return true;
}

bool		Server::acceptConnections() {
	pollfd			pollSock;
	
	pollSock.fd = serverSocket;
	pollSock.events = POLLIN;
	pollSock.revents = 0;
	pollSockets.push_back(pollSock);

	while (true) {
		int	pollReturn = poll(&pollSockets[0], pollSockets.size(), POLL_BLOCK);
		if (pollReturn == -1) {
			std::cerr << "[ERROR]\tPolling failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			// close(serverSocket);
			return (perror("poll"), false);
		}

		for (size_t i = 0; i < pollSockets.size(); i++) {
			if (pollSockets[i].fd == serverSocket) {
				// if event on server socket
				pollServerSocket(pollSockets[i]);
			}
			else {
				// if event on client socket
				pollClientSocket(pollSockets[i]);
			}
		}
	}
}

bool		Server::pollServerSocket(pollfd&	pollServerSock) {
	if (pollServerSock.revents & POLLIN) {
		int	newSocket = accept(serverSocket, NULL, NULL);
		if (newSocket == -1) {
			std::cerr << "[ERROR]\tAccepting failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			perror("accept");
			return false;
		}
		pollfd		newEntry;
		
		newEntry.fd = newSocket;
		newEntry.events = POLLIN;
		newEntry.revents = 0;
		pollSockets.push_back(newEntry);

		std::cout << "[SERVER]\tAccepted Connection From Client "
				  << newEntry.fd << std::endl;
	}
	// handle other revents
	return true;
}

bool		Server::pollClientSocket(pollfd&	pollClientSock) {
	if (pollClientSock.revents & POLLIN) {
		char				buffer[BUFFER_SIZE];
		int	clientSocket = 	pollClientSock.fd;

		memset(buffer, 0, BUFFER_SIZE);
		int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (bytesReceived > 0) {
			std::cout << "[SERVER]\tReceived " << bytesReceived << " bytes from Client "
					  << clientSocket << "..." << std::endl;
		} else if (bytesReceived == 0) {
			// client disconnected
			// close socket
			// remove from pollSockets
		} else if (bytesReceived == -1) {
			std::cerr << "[ERROR]\tReceiving failed..." << std::endl;
			std::cerr << "[ERROR]\t";
			// recv failed
			// close socket
			// remove from pollSockets
			perror("recv");
			return false;
		}
	}
	// handle other revents
}

void		Server::handleRequest() {

}
