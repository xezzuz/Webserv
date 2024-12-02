/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:36:38 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/02 19:38:28 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Response.hpp"

std::string			Response::getRequestedResourceType() {
	std::string			resourcePath = requestedResource;

	struct stat	resourceStats;
	
	if (stat(resourcePath.c_str(), &resourceStats) != 0)
		return "404";
	if (S_ISREG(resourceStats.st_mode))
		return "FILE";
	else if (S_ISDIR(resourceStats.st_mode))
		return "DIR";
	return "403";
}

void				Response::handleFileResource() {
	
}

// bool				Response::directoryContainsIndexFile() {
// 	std::string			path = 
// }

void				Response::handleDirectoryResource() {
	if (requestedResource[requestedResource.size() - 1] != '/') {
		std::cout << "HTTP/1.1 " << 301 << " " << "Moved Permanently" << std::endl;
		std::cout << "Location: " << _Request->getRequestLineSt().uri + '/' << std::endl;
		return ;
	}
	
	// if (directoryContainsIndexFile()) {
		
	// }
}

void				Response::handleGET() {
	std::string			type = getRequestedResourceType();

	std::cout << "client requested resource type: " << type << std::endl;
	if (type == "404") {
		statusCode = 404;
		return ;
	} else if (type == "403") {
		statusCode = 404;
		return ;
	} else if (type == "FILE") {
		handleFileResource();
	} else if (type == "DIR") {
		handleDirectoryResource();
	}
}
