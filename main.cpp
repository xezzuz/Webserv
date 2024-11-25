/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 13:38:38 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Request.hpp"
#include "./Server/Server.hpp"

int main(void) {
	Server		Webserv(55558);
	
	if (!Webserv.getStatus())
		return 1;
	Webserv.startWebserv();
}
