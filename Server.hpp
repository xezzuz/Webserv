/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 17:28:03 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/14 21:37:06 by nazouz           ###   ########.fr       */
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
#include <poll.h>

#include "Request.hpp"
#define PORT 133742
#define POLL_BLOCK -1
#define POLL_NONBLOCK 0

#define BUFFER_SIZE 2048

class Server {
	private:
		int						port;
		bool					status;
		int						serverSocket;
		sockaddr_in				serverAddress;
		std::vector<pollfd> 	pollSockets;

		std::string				rawRequest;
		
		bool	initServer();
		bool	acceptConnections();
		bool	pollServerSocket(pollfd&	pollServerSock);
		bool	pollClientSocket(pollfd&	pollClientSock);
		void	handleRequest();
		
	public:
		Server(const int _port);
		~Server();

		void	startWebserv();
		void	stopWebserv();
};

#endif