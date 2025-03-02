#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include <iostream>
void	isValidIndex(const std::string& index)
{
	std::vector<std::string> indexvec;

	static const std::string empty = " \t\r\n\v\f";
	size_t start = 0;
	size_t end = index.find_first_of(empty, 0);
	std::cout << end << std::endl;

	while (start < index.size())
	{
		indexvec.push_back(index.substr(start, end - start));
		start = index.find_first_not_of(empty, end);
		end = index.find_first_of(empty, start);
	}

	for (size_t i = 0; i < indexvec.size(); i++)
		std::cout << indexvec[i] << std::endl;
}

// std::vector<std::string>	split(const std::string& str, const char *set) {
// 	size_t start = 0;
//     size_t end = str.find_first_not_of(set);
// 	std::vector<std::string>		result;
    
//     while (start < str.size()) {
//         if (end == std::string::npos) {
//             result.push_back(str.substr(start));
//             break;
//         }
        
//         result.push_back(str.substr(start, end - start));
//         start = str.find_first_not_of(set, end);
//         if (start == std::string::npos)
//             break;
        
//         end = str.find_first_of(set, start);
//     }
// 	return result;
// }

std::vector<std::string> split(const std::string& tosplit, const std::string& charset) {
    std::vector<std::string> result;
    size_t start = 0, end;
    while ((end = tosplit.find_first_of(charset, start)) != std::string::npos) {
        if (end > start) {
            result.push_back(tosplit.substr(start, end - start));
        }
        start = end + 1;
    }
    if (start < tosplit.length()) {
        result.push_back(tosplit.substr(start));
    }
    return result;
}

int main()
{
	std::vector<std::string> splitted = split("     :w ", " :");

	for (size_t i = 0; i < splitted.size(); i++) {
		std::cout << "[" << splitted[i] << "]" << std::endl;
	}

    return 0;
}
// void		readCgi(int fd)
// {
// 	char	buf[1024] = {0};
// 	int		bytesRead = read(fd, buf, 1024);
// 	std::cout << "BYTES READ: " << bytesRead << std::endl;
// 	if (bytesRead == -1)
// 	{
// 		std::cerr << "ERRORROROROROOR" << std::endl;
// 	}
// 	if (bytesRead > 0)
// 	{
// 		std::cout << buf;
// 	}
// 	else
// 	{
// 		std::cout << "THE END IS NEVER THE END IS NEVER THE END IS NEVER THE END IS NEVER THE END IS NEVER THE END IS NEVER " << std::endl;
// 	}
// }

// int main()
// {
//     int fd[2];
// 	pipe(fd);
// 	if (fork() == 0)
// 	{
// 		dup2(fd[1], 1);
// 		close(fd[1]);
// 		close(fd[0]);
// 		char *ptr[3];
// 		ptr[0] = "/usr/bin/python3";
// 		ptr[1] = "www/cgi-bin/test.py";
// 		ptr[2] = NULL;
// 		execve(ptr[0], ptr, NULL);
// 	}
// 	close(fd[1]);
// 	while (1)
// 		readCgi(fd[0]);
// }


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