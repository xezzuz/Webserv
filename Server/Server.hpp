/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 17:28:03 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 16:28:28 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
#define BUFFER_SIZE 102400

class Server {
	private:
		int									port;
		bool								status;
		int									serverSocket;
		sockaddr_in							serverAddress;
		
		ServerConfig						defaultConfig;
		std::vector<ServerConfig>			vServerConfigs;

		std::map<int, Client>				Clients;
		

		bool	handleServerSocketEvent(pollfd&	pollServerSock, std::vector<pollfd>& pollSockets);
		bool	handleClientSocketEvent(pollfd&	pollClientSock, std::vector<pollfd>& pollSockets);
		
		void	addToPoll(int fd, short events, short revents, std::vector<pollfd>& pollSockets);
		void	rmFromPoll(int fd, std::vector<pollfd>& pollSockets);
		void	addToClientsMap(int key);
		void	rmFromClientsMap(int key);
		

		void	handleRequest(char *buffer, int bufferSize, int clientSocket);
		
	public:
		Server(std::vector<ServerConfig>& allConfigs);
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