// #include "../Response.hpp"

// std::string			Response::getRequestedResourceType() {
// 	std::string			resourcePath = requestedResource;

// 	struct stat	resourceStats;
	
// 	if (stat(resourcePath.c_str(), &resourceStats) != 0)
// 		return "404";
// 	if (S_ISREG(resourceStats.st_mode))
// 		return "FILE";
// 	else if (S_ISDIR(resourceStats.st_mode))
// 		return "DIR";
// 	return "403";
// }

// void				Response::handleFileResource() {
	
// }

// bool				Response::directoryContainsIndexFile() {
// 	for (size_t i = 0; i < locationBlock->index.size(); i++) {
// 		std::string		indexPath = requestedResource + locationBlock->index[i];
		
// 		struct stat	indexPathStats;
	
// 		if (stat(indexPath.c_str(), &indexPathStats) != 0)
// 			continue;
// 		if (S_ISREG(indexPathStats.st_mode)) {
// 			requestedResource = indexPath;
// 			return true;
// 		}
// 		else if (S_ISDIR(indexPathStats.st_mode))
// 			continue;
// 	}
// 	return false;
// }

// void				Response::handleDirectoryResource() {
// 	if (requestedResource[requestedResource.size() - 1] != '/') {
// 		std::cout << "HTTP/1.1 " << 301 << " " << "Moved Permanently" << std::endl;
// 		std::cout << "Location: " << _Request->getRequestLineSt().uri + '/' << std::endl;
// 		return ;
// 	}
	
// 	if (directoryContainsIndexFile()) {
// 		std::cout << "HTTP/1.1 " << 200 << " " << "OK" << std::endl;
// 		std::cout << "Server will serve: " << requestedResource << std::endl;
// 		return ;
// 	}
	
// 	if (locationBlock->autoindex == "on") {
// 		std::cout << "HTTP/1.1 " << 200 << " " << "OK" << std::endl;
// 		std::cout << "Server will list all files in: " << requestedResource << std::endl;
// 		return ;
// 	}
	
// 	if (locationBlock->autoindex == "off") {
// 		std::cout << "HTTP/1.1 " << 404 << " " << "Not Found" << std::endl;
// 		std::cout << "Server will not list files in: " << requestedResource << std::endl;
// 		return ;
// 	}
// }

// void				Response::handleGET() {
// 	std::string			type = getRequestedResourceType();

// 	std::cout << "client requested resource type: " << type << std::endl;
// 	if (type == "404") {
// 		statusCode = 404;
// 		return ;
// 	} else if (type == "403") {
// 		statusCode = 404;
// 		return ;
// 	} else if (type == "FILE") {
// 		handleFileResource();
// 	} else if (type == "DIR") {
// 		handleDirectoryResource();
// 	}
// }
