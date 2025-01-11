#include <iostream>
#include <sstream>

#include <map>


int main()
{
	std::stringstream ss("/../aa/../");

	std::string a;
	while (getline(ss, a, '/'))
		std::cout << a << std::endl;
}