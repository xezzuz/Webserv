#ifndef _ERROR_HPP
# define _ERROR_HPP

#include <iostream>
#include <vector>
#include "../../Utils/Helpers.hpp"
#include "Response.hpp"

class FatalError : public std::exception
{
public:
	FatalError(const char *msg);

	virtual const char *what() const throw();

private:
	const char	*msg;
};


class ErrorPage : public std::exception, public Response
{
public:
	virtual ~ErrorPage() throw();
	ErrorPage(int code);


	void	generateErrorPage();

private:
	int											status;
	std::map<int, std::string>					description;
};

#endif