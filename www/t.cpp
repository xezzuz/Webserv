#include <iostream>
#include <sstream>

#include <map>

std::string		generateBoundary( void )
{
	std::string alphaNum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	srand(static_cast<size_t>(time(0)));

	std::string random;
	for (int i = 0; i < 10; i++)
	{
		random += alphaNum[rand() % 61];
	}
	return (random);
}

int main()
{
	std::string aa = "Range: bytes=0-499,1000-1499";
	std::cout << generateBoundary() << std::endl;
	std::cout << aa.find("bytes") << std::endl;
}