/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 10:54:09 by nazouz            #+#    #+#             */
/*   Updated: 2025/01/19 20:25:16 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Response/Response.hpp"
#include <algorithm>

class Client {
	private:
		int				socket;
		Request			_Request;
		Response		_Response;
		
		
	public:
		Client();
		Client(int socket);
		Client(const Client& rhs);
		Client& operator=(const Client& rhs);
		~Client();

		Request&		getRequest();
		Response&		getResponse();
		void			resetResponse();
		
		int				getSocket();
		
		void			initResponse(std::vector<vServerConfig>& servers);
		
};

#endif