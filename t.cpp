#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line = "appleorange"; // Example input
    std::istringstream stream(line);
    std::string token;
    char delimiter = ','; // The delimiter you are using

	size_t pos = 0;
	std::cout << "a" << std::endl;
    while (getline(stream, token, ','))
	{

        std::cout << "Token: " << token << std::endl;
    }

    return 0;
}


// int main() {
//     struct timeval start, end;
// 	gettimeofday(&start, NULL);
//     sleep(5);
// 	gettimeofday(&end, NULL);
    
//     double ms = (end.tv_sec - start.tv_sec) * 1000.0;
//     ms += (end.tv_usec - start.tv_usec) / 1000.0;
    
//     std::cout << ms << " milliseconds" << std::endl;
//     return 0;
// }