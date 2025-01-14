#ifndef HELPERS_HPP
# define HELPLERS_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "../Request/Request.hpp"


struct Directives
{
	std::string 								root;
	std::string 								alias;
	std::string 								autoindex;
	std::string 								upload_store;
	std::string 								cgi_pass;
	std::vector<std::string>					index;
	std::vector<std::string>					methods;
	std::vector<std::string>					cgi_ext;
	bool										redirected;
	std::pair<int, std::string> 				redirect;
	std::vector<std::pair<int, std::string> >	error_page;
};

struct	ResponseInput
{
	std::string							uri;
	std::string							method;
	int									status;
	std::map<std::string, std::string>	requestHeaders;
	Directives							config;
};

std::string		stringtrim(const std::string& str, const std::string& set);
bool			isHexa(const std::string& num);
std::string		stringtolower(std::string& str);
bool			stringIsDigit(const std::string& str);
int				hexToInt(const std::string& num);
unsigned int	parseIPv4(const std::string& ipAddress);
std::string		getDate( void );
std::string		getDate( void );
std::string		getContentType(const std::string& target, std::map<std::string, std::string>& mimeTypes);
size_t			fileLength(std::string& path);
std::string		_toString(int num);
std::string		_toString(unsigned long num);
bool			rootJail(const std::string& uri);
std::string		generateRandomString( void );
bool			allDigit(std::string str);


#endif