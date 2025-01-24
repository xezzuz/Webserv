/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/24 17:47:18 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Request.hpp"

#include "./Main/Webserv.hpp"
#include "./Server/Server.hpp"
#include "./Config/Config.hpp"
#include "signal.h"

int main(int argc, char **argv) {
	if (argc != 2)
		return (std::cerr << BOLD << "Webserv: Invalid Number of Arguments!\nUse: ./Webserv <config file>" << RESET << std::endl, 1);
	
	signal(SIGPIPE, SIG_IGN);
	Webserv			Main(argv[1]);

	if (!Main.configurateWebserv())
		return 1;
	Main.startWebserv();
	Main.monitorWebserv();
	
	return 0;
}

#include <sys/epoll.h>

class A
{
public:
	static void registerfd(int fd, uint32_t events)
	{
		struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;
		
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
	}
private:
	static int epoll_fd;
	
};

int A::epoll_fd = -1;