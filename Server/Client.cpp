/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:56:05 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 18:22:09 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {
	
}

Client::Client(int socket) : socket(socket) {
	
}

Client::~Client() {

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

