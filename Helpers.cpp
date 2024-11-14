/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:28:26 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/13 17:05:27 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

std::string		stringtrim(const std::string& str) {
	size_t		first = 0;
	size_t		last = str.size();

	while (str[first] == ' ' || str[first] == '\t')
		first++;
	while (str[last] == ' ' || str[last] == '\t')
		last--;
	return str.substr(first, last - first);
}

std::string		stringtolower(std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
	}
	return str;
}
