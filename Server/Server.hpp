/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 17:28:03 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/24 13:09:02 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <map>
#include <poll.h>

#include "../Request/Request.hpp"
#include "../Config/Config.hpp"
#include "Client.hpp"

#define PORT 133742
#define POLL_BLOCK -1
#define POLL_NONBLOCK 0

// #define BUFFER_SIZE 4096
#define BUFFER_SIZE 16384

class Server {
	private:
		int									port;
		bool								status;
		int									serverSocket;
		sockaddr_in							serverAddress;
		
		std::vector<vServerConfig>			vServerConfigs;

		std::map<int, Client>				Clients;
		

		bool	handleServerSocketEvent(pollfd&	pollServerSock, std::vector<pollfd>& pollSockets);
		bool	handleClientSocketEvent(pollfd&	pollClientSock, std::vector<pollfd>& pollSockets);
		
		void	addToPoll(int fd, short events, std::vector<pollfd>& pollSockets);
		void	rmFromPoll(int fd, std::vector<pollfd>& pollSockets);
		void	addToClientsMap(int key);
		void	rmFromClientsMap(int key);
		

		void	handleRequest(char *buffer, int bufferSize, int clientSocket, pollfd& pollClientSock);
		// void	handleRequest(char *buffer, int bufferSize, int clientSocket);
		
	public:
		Server(std::vector<vServerConfig>& allConfigs);
		Server(const Server& original);
		Server&		operator=(const Server& original);
		~Server();

		bool						initServer();
		// void						startWebserv();
		// void						stopWebserv();

		bool						handleEvent(pollfd& event, std::vector<pollfd>& pollSockets);

		bool						getStatus() { return status; };
		int							getPort() { return port; };
		int							getServerSocket() { return serverSocket; };
		sockaddr_in					getServerAddress() { return serverAddress; };
		std::map<int, Client>&		getClients() { return Clients; };
};

unsigned int	parseIPv4(const std::string& ipAddress);

#endif