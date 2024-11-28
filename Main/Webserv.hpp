/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 15:07:53 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/28 13:57:31 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server/Server.hpp"
#include "../Server/Client.hpp"
#include "../Config/Config.hpp"

class Webserv {
	private:
		Config							WebservConfig;

		std::vector<Server>				Servers;
		std::vector<pollfd> 			pollSockets;
		
	public:
		Webserv(const std::string& configFileName);
		~Webserv();

		bool			configurateWebserv();
		bool			startWebserv();
		bool			monitorWebserv();

		void			addToPoll(int fd, short events, short revents);

		bool			isServerSocket(const int socket);
		
		Config&			getWebservConfig();
};
