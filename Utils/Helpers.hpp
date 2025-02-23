#ifndef HELPERS_HPP
# define HELPLERS_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <map>

class Disconnect : public std::exception
{
public:
	virtual ~Disconnect() throw() {}
	Disconnect(std::string msg) : msg(msg) {}

	virtual const char *what() const throw() { return (msg.c_str()); }

private:
	std::string	msg;
};

class CGIRedirectException : public std::exception
{
public:
	virtual ~CGIRedirectException() throw() {}
	CGIRedirectException(const std::string& location) : location(location) {}

	std::string location;
};

std::string		stringtrim(const std::string& str, const std::string& set);
bool			isHexa(const std::string& num);
std::string		stringtolower(std::string str);
bool			stringIsDigit(const std::string& str);
int				hexToInt(const std::string& num);
unsigned int	parseIPv4(const std::string& ipAddress);
std::string		getDate( void );
std::string		getDate( void );
std::string		getContentType(const std::string& target, std::map<std::string, std::string>& mimeTypes);
long			fileLength(std::string& path);
std::string		_toString(int num);
std::string		_toString(long num);
std::string		_toString(unsigned long num);
bool			rootJail(const std::string& uri);
std::string		generateRandomString( void );
bool			allDigit(std::string str);
std::string		capitalize(const std::string& word);
std::string		toHex(size_t num);
std::string		buildChunk(const char *data, size_t size); // error
void printMap(std::map<std::string, std::string>& map);

#endif