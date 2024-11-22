/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 17:28:03 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/21 19:35:59 by nazouz           ###   ########.fr       */
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
#include "Client.hpp"

#define PORT 133742
#define POLL_BLOCK -1
#define POLL_NONBLOCK 0

#define BUFFER_SIZE 4096

class Server {
	private:
		int									port;
		bool								status;
		int									serverSocket;
		sockaddr_in							serverAddress;
		std::vector<pollfd> 				pollSockets;

		std::map<int, Client>				clients;
		
		bool	initServer();
		
		bool	acceptConnections();

		bool	pollServerSocket(pollfd&	pollServerSock);
		bool	pollClientSocket(pollfd&	pollClientSock);
		
		pollfd&	addToPoll(pollfd& toAdd, int fd, short events, short revents);
		void	rmFromPoll(pollfd&	toRemove);
		void	rmFromClientsMap(int key);
		
		void	handleRequest(char *buffer, int bufferSize, int clientSocket);
		
	public:
		Server(const int _port);
		~Server();

		void	startWebserv();
		void	stopWebserv();

		bool	getStatus() { return status; };
};

#endif