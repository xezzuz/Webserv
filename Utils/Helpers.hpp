#ifndef HELPERS_HPP
# define HELPLERS_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include "../Request/Request.hpp"


struct	ResponseInput
{
	std::string											uri;
	std::string											method;
	int													status;
	std::map<std::string, std::string>					requestHeaders;
	std::map<std::string, std::vector<std::string> >	config;
};

std::string		stringtrim(const std::string& str, const std::string& set);
bool			isHexa(const std::string& num);
std::string		stringtolower(std::string& str);
bool			stringIsDigit(const std::string& str);
int				hexToInt(const std::string& num);
unsigned int	parseIPv4(const std::string& ipAddress);
std::string		getDate( void );
std::string		getDate( void );
std::string		contentType(const std::string& target, std::map<std::string, std::string>& mimeTypes);
size_t			fileLength(std::string& path);
std::string		_toString(int num);
std::string		_toString(unsigned long num);

#endif