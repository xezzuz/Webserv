/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/28 11:48:46 by mmaila           ###   ########.fr       */
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