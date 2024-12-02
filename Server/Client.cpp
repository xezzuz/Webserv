/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/02 19:30:21 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
	// std::cout << "Client::Constructor Called" << std::endl;
}

Client::Client(int socket) : socket(socket) {
	// std::cout << "Client::Param Constructor Called" << std::endl;
}

Client::~Client() {
	// std::cout << "Client::Destructor Called" << std::endl;
}

Request&		Client::getRequest() {
	return _Request;
}

Response&		Client::getResponse() {
	return _Response;
}

int				Client::getSocket() {
	return socket;
}

