/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:06:37 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/27 22:34:26 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::vector<vServerConfig>& Configs) : status(false), serverSocket(-1), vServerConfigs(Configs) {
	
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
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "[SERVER]\tCreating a Socket failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		return (perror("socket"), false);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(vServerConfigs[0].port);
	serverAddress.sin_addr.s_addr = parseIPv4(vServerConfigs[0].host);
	
	int reUseAddr = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reUseAddr, sizeof(reUseAddr)); // check return value?
	
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "[SERVER]\tBinding failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		close(serverSocket);
		return (perror("bind"), false);
	}
	
	if (listen(serverSocket, 128) == -1) {
		// std::cerr << "[SERVER]\tListening failed..." << std::endl;
		std::cerr << "[SERVER]\t";
		close(serverSocket);
		return (perror("listen"), false);
	}

	// set non blocking mode and close the fd on execve
	if (fcntl(serverSocket, F_SETFL, FD_CLOEXEC | O_NONBLOCK) == -1)
	{
		std::cerr << "[WEBSERV]\t" << std::endl;
		perror("fcntl");
		close(serverSocket);
		return (false);
	}
	status = true;
	std::cout << "[SERVER]\tListening on " << vServerConfigs[0].host << ":" << vServerConfigs[0].port << "..." << std::endl;
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

		// set non blocking mode and close the fd on execve
		if (fcntl(serverSocket, F_SETFL, FD_CLOEXEC | O_NONBLOCK) == -1)
		{
			std::cerr << "[WEBSERV]\t" << std::endl;
			perror("fcntl");
			close(serverSocket);
			return (false);
		}

		addToPoll(newSocket, POLLIN | POLLHUP, pollSockets);
		addToClientsMap(newSocket);

		std::cout << "[SERVER]\tAccepted Connection from Client "
				  << newSocket << "..." << std::endl;
	}
	 else {
		std::cout << "Another event besides POLLIN" << std::endl;
	}
	// handle other revents
	return true;
}

bool		Server::handleClientSocketEvent(pollfd&	pollClientSock, std::vector<pollfd>& pollSockets) {
	int					clientSocket = pollClientSock.fd;
	if (pollClientSock.revents & POLLIN)
	{
		if (Clients.find(clientSocket) == Clients.end())
		{
			int retVal = Clients[clientSocket].getResponse().sendResponse(clientSocket);
			if(retVal == -1) // can be || (retval == 1 && client.(connection header is "close"))
			{
				//disconnectClient();
				rmFromClientsMap(clientSocket);
				rmFromPoll(clientSocket, pollSockets);
				std::cout << "CLIENT REMOVED" << std::endl;
			}
			else if (retVal == 1)
			{
				std::cout << "CLIENT " << clientSocket << " SERVED. RESETING.." << std::endl;
				Clients[clientSocket].reset();
				pollClientSock.events = POLLIN | POLLHUP;
			}
		}
		else
		{
			char				buffer[BUFFER_SIZE + 1];

			memset(buffer, 0, BUFFER_SIZE + 1);
			int	bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
			if (bytesReceived > 0) {
				std::cout << "----------REQUEST_OF_CLIENT " << clientSocket << "----------" << std::endl;
				std::cout << buffer << std::endl;
				std::cout << "---------------------------------------------------------" << std::endl;
				handleRequest(buffer, bytesReceived, clientSocket, pollClientSock);
			} else if (bytesReceived == 0) { // this is for graceful shutdown (client closes the connection willingly)
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
		}
	} else if (pollClientSock.revents & POLLOUT) {

		// if (Clients[clientSocket].getResponse().getResponseIsReady())
		int retVal = Clients[clientSocket].getResponse().sendResponse(clientSocket);
		if(retVal == -1) // can be || (retval == 1 && client.(connection header is "close"))
		{
			//disconnectClient();
			rmFromClientsMap(clientSocket);
			rmFromPoll(clientSocket, pollSockets);
			std::cout << "CLIENT REMOVED" << std::endl;
		}
		else if (retVal == 1)
		{
			std::cout << "CLIENT " << clientSocket << " SERVED. RESETING.." << std::endl;
			Clients[clientSocket].reset();
			pollClientSock.events = POLLIN | POLLHUP;
		}
		
		// 	send(clientSocket, Clients[clientSocket].getResponse().getRawResponse().c_str(), Clients[clientSocket].getResponse().getRawResponse().size(), 0);
		// close(clientSocket);
        
	}
	else if (pollClientSock.revents & POLLHUP) // this is for abrupt disconnections (connection lost, client crash ...)
	{
		rmFromClientsMap(clientSocket);
		rmFromPoll(clientSocket, pollSockets);
		std::cout << "------------------------CLIENT DISCONNECTED----------------------------------------------" << std::endl;
		return (false);
	}
	// handle other events
	return true;
}

void		Server::handleRequest(char *buffer, int bufferSize, int clientSocket, pollfd& pollClientSock) {
	std::map<int, Client>::iterator	it = Clients.find(clientSocket);

	if (it == Clients.end()) {
		std::cout << "client was not found!" << std::endl; // should remove the socket from the poll
		return ;
	}
	// could be optimized using iterators
	Clients[clientSocket].getRequest().feedRequest(buffer, bufferSize);
	if (Clients[clientSocket].getRequest().getParsingState() == PARSING_FINISHED)
    {
		pollClientSock.events = POLLOUT | POLLHUP;
		// std::cout << Clients[clientSocket].getRequest().getRequestLineSt().uri << std::endl;
		Clients[clientSocket].initResponse(vServerConfigs);
		return ;
	}
}

void		Server::addToPoll(int fd, short events, std::vector<pollfd>& pollSockets) {
	pollfd			toAdd;

	toAdd.fd = fd;
	toAdd.events = events;
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

void		Server::addToClientsMap(int key)
{
	Clients.insert(std::make_pair(key, Client(key)));
	// Clients[key] = Client(key);
}

void		Server::rmFromClientsMap(int key) {
	std::map<int, Client>::iterator	it;

	it = Clients.find(key);
	if (it == Clients.end())
		return ;
	Clients.erase(it);
}
