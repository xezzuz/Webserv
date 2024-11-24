/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:02 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/24 20:23:35 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>

Config::Config() {
	
}

Config::Config(const std::string& configFileName) {
	if (!openConfigFile(configFileName)) {
		std::cerr << "Error Opening Config File..." << std::endl;
		return ;
	}
	
	if (!parseConfigFile()) {
		std::cerr << RED << "Config File is not valid..." << std::endl;
		return ;
	}
	std::cerr << GREEN << "Config File is valid..." << std::endl;
}

Config::~Config() {
	
}
