#include <iostream>
#include <sstream>
#include <dirent.h>
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
	DIR *t = opendir(".");

	struct dirent	*entry;

	while((entry = readdir(t)) != nullptr)
	{
		if (entry->d_type == DT_DIR)
			std::cout << "DIR";
		else if (entry->d_type == DT_REG)
			std::cout << "FILE";
		std::cout << ": "<< entry->d_name << std::endl;
	}
}