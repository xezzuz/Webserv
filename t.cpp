#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>

#include <iostream>

class Response {
public:
    // Define a function pointer type for Response class
    void (Response::*reader)();

    // Constructor
    Response() : reader(nullptr) {}

	void resRead(){
		std::cout << "res Read" << std::endl;
	}
    // Set function pointer
    void setFunction(void (Response::*func)()) {
        reader = func;
    }

    // Call function through the pointer
    void callFunction() {
        if (reader) {
            (this->*reader)();
        } else {
            std::cout << "No function assigned.\n";
        }
    }
};

class CGIHandler : public Response
{
public:
	CGIHandler()
	{
		reader = reinterpret_cast<void (Response::*)()>(&CGIHandler::readCgi);
	}
    void readCgi() {
        std::cout << "CGIHandler readCgi function\n";
    }
};

int main()
{
    Response response;
    CGIHandler cgiHandler;

    // Cast the function pointer from CGIHandler to Response
    // response.setFunction(reinterpret_cast<void (Response::*)()>(&CGIHandler::readCgi));

    // Now call the function (this would ideally work if set up properly)
    cgiHandler.callFunction();

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