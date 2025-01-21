/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:07:53 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/21 10:50:07 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server/Server.hpp"
#include "../Server/Client.hpp"
#include "../Config/Config.hpp"

class Webserv {
	private:
		Config							WebservConfig;

		std::vector<Server>				vServers;
		// std::map<int, Client>			Clients;
		std::vector<pollfd> 			pollSockets;
		
	public:
		Webserv(const std::string& configFileName);
		~Webserv();

		bool			configurateWebserv();
		bool			startWebserv();
		bool			monitorWebserv();

		Server*			getResponsibleServer(int fd);
		bool			handleServerSocketEvents(pollfd&	pollServerSock);
		bool			handleClientSocketEvents(pollfd&	pollClientSock);
		void			handleRequest(char *buffer, int bufferSize, int clientSocket);

		void			addToPoll(int fd, short events);
		void			rmFromPoll(int fd);
		void			rmFromClientsMap(int key);

		bool			isServerSocket(const int socket);
		
		Config&			getWebservConfig();
};
