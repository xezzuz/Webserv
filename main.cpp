/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:40:52 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/21 19:51:48 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Request.hpp"
#include "./Server/Server.hpp"

int main(void) {
	std::string		httpRequest = 
    "GET /index.html HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "User-Agent: MyHttpClient/1.0\r\n"
	"Content-Type: application/json\r\n"
	"Content-Length: 18\r\n"
	"Transfer-Encoding: CHUNKED\r\n"
    "Accept: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
	"<html>"
	"Welcome to the <img src=”/logo.gif”> example.re homepage!"
	"</html>";
	// parse config file

	// socket programming

	// recv request
	// parse request
	// Request		receivedRequest(httpRequest);
	Server		Webserv(55559);
	
	if (!Webserv.getStatus())
		return 1;
	Webserv.startWebserv();
}
