/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 16:34:22 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/24 20:03:30 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
// #include <sstream>
// #include <iostream>

int main(int argc, char **argv) {
	if (argc < 2)
		return 1;
	
	Config		configfile(argv[1]);
	
	return 0;
}