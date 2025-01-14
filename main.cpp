/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/30 15:40:06 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Request.hpp"

#include "./Main/Webserv.hpp"
#include "./Server/Server.hpp"
#include "./Config/Config.hpp"

int main(int argc, char **argv) {
	if (argc != 2)
		return (std::cerr << BOLD << "Webserv: Invalid Number of Arguments!\nUse: ./Webserv <config file>" << RESET << std::endl, 1);
	
	Webserv			Main(argv[1]);

	if (!Main.configurateWebserv())
		return 1;
	Main.startWebserv();
	Main.monitorWebserv();
	
	return 0;
}
