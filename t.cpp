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
	// DIR *t = opendir(".");

	// struct dirent	*entry;

	// while((entry = readdir(t)) != nullptr)
	// {
	// 	if (entry->d_type == DT_DIR)
	// 		std::cout << "DIR";
	// 	else if (entry->d_type == DT_REG)
	// 		std::cout << "FILE";
	// 	std::cout << ": "<< entry->d_name << std::endl;
	// }
	struct stat file;

	if(stat("/home/mmaila/Desktop/SERV/www/favicon.ico", &file) == -1)
		std::cout << "NOT FOUND" << std::endl;
	std::cout  << toHex(45384)  << std::endl;
}