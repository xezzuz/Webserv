#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


std::string						resolveURITraversal(std::string& URI) {
	std::stringstream			ss(URI);
	std::vector<std::string>	splittedPath;
	std::string					token;

	if (URI.empty() || URI == "/")
		return URI;
	
	if (URI.find("/..") == URI.size() - 3 || URI.find("/.") == URI.size() - 2)
		URI += "/";
	
	while (std::getline(ss, token, '/')) {
		if (token == "" || token == ".")
			continue;
		if (token == "..") {
			if (splittedPath.size())
				splittedPath.pop_back();
		}
		else
			splittedPath.push_back(token);
	}

	std::string		resolvedURI = "/";
	for (size_t i = 0; i < splittedPath.size(); i++)
		resolvedURI += splittedPath[i] + "/";

	if (resolvedURI.size() > 1) {
		if (URI[URI.size() - 1] == '/')
			URI = resolvedURI;
		else
			resolvedURI.pop_back(), URI = resolvedURI;
	}
	return resolvedURI;
}


int main(int arcg, char **argv)
{
	std::string tmp = argv[1];

	std::cout << "Resolved Path: " << resolveURITraversal(tmp) << std::endl;
}