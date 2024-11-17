/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:06:37 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/17 18:01:47 by nazouz           ###   ########.fr       */
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
	acceptConnections();
}

void		Server::stopWebserv() {

}

bool		Server::initServer() {
	std::cout << "[SERVER]\tStarting the Webserv..." << std::endl;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "[ERROR!]\tCreating a Socket failed..." << std::endl;
		std::cerr << "[ERROR!]\t";
		return (perror("socket"), false);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "[ERROR!]\tBinding failed..." << std::endl;
		std::cerr << "[ERROR!]\t";
		close(serverSocket);
		return (perror("bind"), false);
	}
	
	if (listen(serverSocket, 128)) {
		std::cerr << "[ERROR!]\tListening failed..." << std::endl;
		std::cerr << "[ERROR!]\t";
		close(serverSocket);
		return (perror("listen"), false);
	}
	
	std::cout << "[SERVER]\tWebserv is listening on port " << port << "..." << std::endl;
	return true;
}

bool		Server::acceptConnections() {
	pollfd			pollSock;
	
	addToPoll(pollSock, serverSocket, POLLIN, 0);

	while (true) {
		int	pollReturn = poll(&pollSockets[0], pollSockets.size(), POLL_BLOCK);
		if (pollReturn == -1) {
			std::cerr << "[ERROR!]\tPolling failed..." << std::endl;
			std::cerr << "[ERROR!]\t";
			// close(serverSocket);
			return (perror("poll"), false);
		}

		for (size_t i = 0; i < pollSockets.size(); i++) {
			if (pollSockets[i].fd == serverSocket) {
				// if event on server socket
				if (!pollServerSocket(pollSockets[i]))
					continue;
			}
			else {
				// if event on client socket
				if (!pollClientSocket(pollSockets[i]))
					i--;
			}
		}
	}
}

bool		Server::pollServerSocket(pollfd&	pollServerSock) {
	if (pollServerSock.revents & POLLIN) {
		int	newSocket = accept(serverSocket, NULL, NULL);
		if (newSocket == -1) {
			std::cerr << "[ERROR!]\tAccepting failed..." << std::endl;
			std::cerr << "[ERROR!]\t";
			perror("accept");
			return false;
		}
		pollfd		newEntry;
		
		addToPoll(newEntry, newSocket, POLLIN, 0);

		std::cout << "[SERVER]\tAccepted Connection from Client "
				  << newEntry.fd << "..." << std::endl;
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
			// std::cout << "[SERVER]\tReceived [" << bytesReceived << "] bytes from Client "
			// 		  << clientSocket << "..." << std::endl;
			handleRequest(buffer, clientSocket);
		} else if (bytesReceived == 0) {
			std::cout << "[SERVER]\tClient " << clientSocket
					  << " disconnected..." << std::endl;
			// client disconnected
			rmFromPoll(pollClientSock);
			rmFromClientsRequests(clientSocket);
			return false;
		} else if (bytesReceived == -1) {
			std::cerr << "[ERROR!]\tReceiving failed..." << std::endl;
			std::cerr << "[ERROR!]\t";
			// recv failed
			// close socket
			// remove from pollSockets
			rmFromPoll(pollClientSock);
			perror("recv");
			return false;
		}
	}
	// handle other revents
	return true;
}

pollfd&		Server::addToPoll(pollfd& toAdd, int fd, short events, short revents) {
	toAdd.fd = fd;
	toAdd.events = events;
	toAdd.revents = revents;
	pollSockets.push_back(toAdd);
	return toAdd;
}

void		Server::rmFromPoll(pollfd&	toRemove) {
	for (size_t i = 0; i < pollSockets.size(); i++) {
		if (&pollSockets[i] == &toRemove) {
			close(pollSockets[i].fd);
			std::vector<pollfd>::iterator	it = pollSockets.begin() + i;
			pollSockets.erase(it);
			return ;
		}
	}
}

void		Server::handleRequest(const std::string& buffer, int clientSocket) {
	std::map<int, Request>::iterator	it = clientsRequests.find(clientSocket);
	if (it == clientsRequests.end()) {
		// create a newEntry
		// std::cout << "[REQUEST]\tParsing a new Client Request " << clientSocket << "..." << std::endl;
		clientsRequests.insert(std::make_pair(clientSocket, Request(buffer)));
		clientsRequests[clientSocket].requestParseControl();
	} else {
		// update the old request
		// std::cout << "[REQUEST]\tParsing a old Client Request " << clientSocket << "..." << std::endl;
		if ( it->second.getHeadersParsed() && 
			 it->second.getRequestLineSt().rawRequestLine == "GET")
			return ;
		if (it->second.getBodyParsed())
			return ;
		it->second.setBuffer( it->second.getBuffer().append(buffer) );
		it->second.requestParseControl();
	}
}

void		Server::rmFromClientsRequests(int key) {
	std::map<int, Request>::iterator	it;

	it = clientsRequests.find(key);
	if (it != clientsRequests.end())
		return ;
	clientsRequests.erase(it);
}