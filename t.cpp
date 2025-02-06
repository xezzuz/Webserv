#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>

int main() {
    int i;

    bool b = false;

    std::cout << i << std::endl;
    if (b)
        std::cout << "true" << std::endl;
    else
        std::cout << "false" << std::endl;
    std::cout << b << std::endl;

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