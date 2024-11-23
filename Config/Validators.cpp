/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validators.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 18:00:37 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/23 19:55:04 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool			isValidPort(const std::string& port) {
	if (port.empty() || port.size() > 5)
		return false;
	for (size_t i = 0; i < port.size(); i++) {
		if (!isdigit(port[i]))
			return false;
	}
	
	int		portDecimal = std::atoi(port.c_str());	
	if (portDecimal > 0 && portDecimal < 65535)
		return true;
	return false;
}

bool					isValidHost(const std::string& host) {
	std::string					byte;
	std::stringstream			ss(host);
	int							bytesCount = 0;

	while (std::getline(ss, byte, '.')) {
		if (++bytesCount > 4)
			return false;
		if (byte.empty() || byte.size() > 3)
			return false;
		for (size_t i = 0; i < byte.size(); i++) {
			if (!isdigit(byte[i]))
				return false;
		}
		if (byte.size() > 1 && byte[0] == '0')
			return false;
		int byteDecimal = std::atoi(byte.c_str());
		if (byteDecimal < 0 || byteDecimal > 255)
			return false;
	}
	return bytesCount == 4;
}

bool					isValidServerName(const std::string& serverName) {
	std::string				sub;
	std::stringstream		ss(serverName);
	int						subCount = 0;
	
	if (serverName.empty() || serverName.size() > 253)
		return false;
	
	while (std::getline(ss, sub, '.')) {
		if (++subCount > 127)
			return false;
		if (sub.empty() || sub.size() > 63)
			return false;
		if (!isalnum(sub[0]) || !isalnum(sub[sub.size() - 1]))
			return false;
		for (size_t i = 0; i < sub.size(); i++) {
			if (!isalnum(sub[i]) && sub[i] != '-')
				return false;
		}
	}
	return subCount >= 2;
}

bool					isValidErrorPage(const std::string& errorPage) {
	return true;
}
