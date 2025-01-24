#include <iostream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <map>
// #include "Utils/Helpers.hpp"

std::string		generateBoundary( void )
{
	std::string alphaNum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	srand(static_cast<size_t>(time(0)));

	std::string random;
	for (int i = 0; i < 10; i++)
	{
		random += alphaNum[rand() % 61];
	}
	return (random);
}

std::string		toHex(size_t num)
{
	std::stringstream ss;

	ss << std::uppercase << std::hex << num;
	return (ss.str());
}
#include <sys/stat.h>
int main()
{
	std::string uri = "/cgi-bin/hello.cgi/path";

	size_t pos = uri.find(".cgi");
	uri.substr(0, pos + 4);

	// std::string queryString = uri.substr(uri.find('?'));
	std::cout << uri.substr(0, pos + 4) << std::endl;
}