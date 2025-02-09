#ifndef _ERROR_HPP
# define _ERROR_HPP

#include <iostream>
#include <vector>
#include "../../Utils/Helpers.hpp"
#include "Response.hpp"

class Disconnect : public std::exception
{
public:
	virtual ~Disconnect() throw() {}
	Disconnect(std::string msg);

	virtual const char *what() const throw();

private:
	std::string	msg;
};

class CGIRedirectException : public std::exception
{
public:
	virtual ~CGIRedirectException() throw();
	CGIRedirectException(const std::string& location);

	std::string location;
};

#endif