#include <iostream>
#include <vector>

int main()
{
	std::vector<int> i;

	i.push_back(1);
	i.push_back(3);
	i.push_back(2);
	i.push_back(5);
	i.push_back(-5);
	i.push_back(-2);
	i.push_back(13);
	i.push_back(0);
	i.push_back(6);

	for (std::vector<int>::iterator it = i.begin(); it != i.end(); )
	{
		if (*it > 3)
			it = i.erase(it);
		else
			it++;
	}
	for (std::vector<int>::iterator it = i.begin(); it != i.end(); it++)
	{
		std::cout << *it << std::endl;
	}

	
}