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

#endif