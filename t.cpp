#include <iostream>
#include <sys/socket.h>

int main()
{
	std::string str = "500";

	std::string a;
	std::string b;
	size_t splitPos = str.find_first_of(' ');
	if (splitPos != std::string::npos)
		b = str.substr(splitPos + 1);
	a = str.substr(0, splitPos);
	std::cout << "A: " << a << "|B:" << b <<std::endl;  
}