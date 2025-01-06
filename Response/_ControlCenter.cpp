#include "Response.hpp"

// void				Response::setResponsibleConfig(std::vector<ServerConfig>& vServerConfigs) {
// 	std::string			host = _Request->getHeaderSt().host;
	
// 	for (size_t i = 0; i < vServerConfigs.size(); i++) {
// 		for (size_t j = 0; j < vServerConfigs[i].server_name.size(); j++) {
// 			if (vServerConfigs[i].server_name[j] == host) {
// 				_Config = vServerConfigs[i];
// 				std::cout << "matching server block was found! " << i << std::endl;
// 				return ;
// 			}
// 		}
// 	}
// 	_Config = vServerConfigs[0];
// 	std::cout << "no matching server block was found! falling back to default server" << std::endl;
// }

// void				Response::setRequestedResource() {
// 	std::string			uri = _Request->getRequestLineSt().uri;

//     std::cout << uri << std::endl;
// 	requestedResource = locationBlock->root; // sigsev here
// 	requestedResource += uri.substr(locationBlock->location.size());

// }

// bool				Response::isMethodAllowed()
// {
// 	for (size_t i = 0; i < locationBlock->methods.size(); i++) {
// 		if (this->components.method == locationBlock->methods[i])
// 			return true;
// 	}
// 	return false;
// }


