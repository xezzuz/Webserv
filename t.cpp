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
#include <vector>
#include <unistd.h>
int main()
{					                         //dir    /cgiScript/pathinfo/
	std::string str = "/home/mmaila/Desktop/index.php/";
	if (chdir(str.c_str()) == -1)
	{
		std::cerr << "chdir failed" << std::endl;
	}
	std::cerr << "dir changed" << std::endl;
	// std::map<std::string, std::string> cgiExt;
	// cgiExt.insert(std::make_pair(".php", "/usr/bin/php"));
	// cgiExt.insert(std::make_pair(".py", "/usr/bin/python3"));
	// std::string method = "GET";
	// bool autoindex = true;
	// bool isDir = false;
	// std::vector<std::string> index;
	// index.push_back("index.html");
	// index.push_back("index.php");

	// struct stat pathStat;
	// size_t pos;


	// std::string path;

	// while (!str.empty())
	// {
	// 	pos = str.find('/', 1);
	// 	path.append(str.substr(0, pos));
	// 	str.erase(0, pos++);
	// 	std::cout << "PATH: " << path << std::endl;
	// 	if (stat(path.c_str(), &pathStat) == -1)
	// 	{
	// 		// input.status = 404;
	// 		std::cout << "404 NOT FOUND" << std::endl;
	// 		return(0);
	// 	}
	// 	if (!S_ISDIR(pathStat.st_mode))
	// 		break;
	// }

	// if (S_ISDIR(pathStat.st_mode))
	// {
	// 	isDir = true;
	// 	if (path.back() != '/')
	// 		path.append("/");
	// 	std::cout << path << std::endl;
	// 	std::cout << "PATH IS DIR" << std::endl;
	// 	if (access(path.c_str(), X_OK) != 0)
	// 	{
	// 		// input.status = 403;
	// 		std::cout << "403 FORBIDDEN" << std::endl;
	// 		return (0);
	// 	}
	// 	if (method == "GET")
	// 	{

	// 		std::vector<std::string>::iterator	it;

	// 		for (it = index.begin(); it != index.end(); it++)
	// 		{
	// 			if (access((path + *it).c_str(), F_OK) == 0) // file exists
	// 			{
	// 				path.append(*it);
	// 				isDir = false;
	// 				break;
	// 			}
	// 		}
	// 		if (it == index.end())
	// 		{
	// 			if (!autoindex)
	// 			{
	// 				// input.status = 404;
	// 				std::cout << "403 FORBIDDEN" << std::endl;
	// 			}
	// 			else
	// 				std::cout << "AUTOINDEX" << std::endl;
	// 			return (0);
	// 		}
	// 	}
	// }

	// std::string scriptName;
	// std::map<std::string, std::string>::iterator it;
	// size_t posi;
	// scriptName = path.substr(path.find_last_of('/'));
	// if ((posi = scriptName.find('.')) != std::string::npos) // REDO
	// {
	// 	std::map<std::string, std::string>::iterator it = cgiExt.find(scriptName.substr(posi));
	// 	if (it != cgiExt.end())
	// 	{

	// 		std::cout << "CGI FOUND >> " << path << std::endl;
	// 		std::cout << "PATH INFO >> " << str << std::endl;
	// 		return (0);
	// 	}
	// }
	// if (str.size() > 0)
	// {
	// 	// status = 404;
	// 	std::cout << "404 NOT FOUND" << std::endl;
	// 	return (0);
	// }


	// if (access(path.c_str(), R_OK) != 0)
	// {
	// 	//status = 403;
	// 	std::cout << "403 FORBIDDEN" << std::endl;
	// }
	
	// std::cout << "PATH: " << path << std::endl;





	// for (std::vector<std::pair<std::string, std::string>>::iterator it = cgiExt.begin(); it != cgiExt.end(); it++)
	// {
	// 	size_t pos = 0;
	// 	while ((pos = str.find(it->first, pos)) != std::string::npos)
	// 	{
	// 		pos += it->first.length();
	// 		std::cout << pos << std::endl;
	// 		std::string	cgiPath = str.substr(0, pos);
	// 		struct stat pathStat;
	// 		if (stat(cgiPath.c_str(), &pathStat) == -1)
	// 		{
	// 			//status = 404;
	// 			std::cout << cgiPath << std::endl;
	// 			std::cerr << "Requested Resource Not Found!" << std::endl;
	// 			return(0);
	// 		}
	// 		if (S_ISREG(pathStat.st_mode))
	// 		{
	// 			// input.cgi.isCgi = true;
	// 			// input.cgi.ext = it->first;
	// 			// input.cgi.interpreter = it->second;
	// 			std::cout << str.substr(str.find_last_of('/')) << std::endl;
	// 			std::cout << "Requested Resource is CGI : " << cgiPath << std::endl;
	// 			//input.cgi.pathInfo = str.substr(cgiPath.length());
	// 			std::cout << "Path info : " << str.substr(cgiPath.length()) << std::endl;
	// 			break;
	// 		}
	// 		std::cout << "failed attemt : " << cgiPath << std::endl;
	// 	}
	// }
}