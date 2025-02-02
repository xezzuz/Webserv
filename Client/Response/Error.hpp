#ifndef _ERROR_HPP
# define _ERROR_HPP

#include <iostream>
#include "../../Utils/Helpers.hpp"

class FatalError : public std::exception
{
public:
	FatalError(const char *msg);

	virtual const char *what() const throw();

private:
	const char	*msg;
};


class ErrorPage : public std::exception
{
public:
	virtual ~ErrorPage() throw();
	ErrorPage(int code);

	std::string	getBuffer( void ) const;
	void		generateErrorPage();

private:
	std::string					path;
	std::string					buffer;
	int							status;
	std::map<int, std::string>	description;
};

#endif