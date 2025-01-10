#include <iostream>

#include <map>

typedef struct	ResponseInput
{
	std::string							target;
	std::string							method;
	int									status;
	std::map<std::string, std::string>	requestHeaders;
} ResponseInput;

int main()
{

}