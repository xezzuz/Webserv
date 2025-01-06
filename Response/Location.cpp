// #include "Response.hpp"

// void				Response::findExactMatchingLocationBlock()
// {
// 	for (size_t i = 0; i < _Config.locations.size(); i++) {
// 		if (_Config.locations[i].location == _Request->getRequestLineSt().uri) {
// 			locationBlock = &_Config.locations[i];
// 			return ;
// 		}
// 	}
// 	locationBlock = NULL;
// }

// void				Response::findLongestMatchingPrefixLocationBlock() {
// 	std::string			uri = _Request->getRequestLineSt().uri;
	
// 	for (size_t i = 0; i < _Config.locations.size(); i++) {
// 		std::string		route = _Config.locations[i].location;

// 		if (uri.find(route, 0) != std::string::npos) {
// 			if (locationBlock && route.size() > locationBlock->location.size())
// 				locationBlock = &_Config.locations[i];
// 			else if (!locationBlock)
// 				locationBlock = &_Config.locations[i];
// 		}
// 	}
// }

// void				Response::setMatchingLocationBlock() {
// 	if (_Config.locations.size() == 0) {
// 		locationBlock = NULL;
// 		return ;
// 	}
// 	findExactMatchingLocationBlock();
// 	if (locationBlock) {
// 		std::cout << "Exact matching location block: " << locationBlock->location << std::endl;
// 		return ;
// 	}

// 	findLongestMatchingPrefixLocationBlock();
// 	if (!locationBlock)
// 		std::cout << "Best matching location block: NULL" << std::endl;
// 	std::cout << "Best matching location block: " << locationBlock->location << std::endl;
// }

// bool				Response::locationHasRedirection() {
// 	if (locationBlock->redirect[0] == "none")
// 		return false;
// 	return true;
// }
