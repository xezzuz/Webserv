/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:54:09 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 18:37:45 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Response/Response.hpp"

class Client {
	private:
		int				socket;
		Request			_Request;
		Response		_Response;
		
		
	public:
		Client();
		Client(int socket);
		~Client();

		Request&		getRequest();
		Response&		getResponse();
		int				getSocket();
};

#endif