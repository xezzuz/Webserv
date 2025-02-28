/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/06 18:35:10 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Request.hpp"

#include "./HTTPServer/Webserv.hpp"
#include "./_Config/Config.hpp"
#include "signal.h"

int main(int argc, char **argv) {
	if (argc != 2)
		return (std::cerr << BOLD << "[WEBSERV]\t usage: Webserv <config file>" << RESET << std::endl, 1);
	
	signal(SIGPIPE, SIG_IGN);
	Config			conf(argv[1]);
	Webserv			Main(conf.getServers());

	Main.initServers();
	Main.run();
	
	return 0;
}