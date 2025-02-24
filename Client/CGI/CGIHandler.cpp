#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"

CGIHandler::~CGIHandler()
{
	HTTPserver->removeHandler(pipe_in);
	HTTPserver->removeHandler(pipe_out);

	if (waitpid(pid, NULL, WNOHANG) == 0)
		kill(pid, SIGTERM);
}

CGIHandler::CGIHandler(int& clientSocket, RequestData *data) : AResponse(clientSocket, data), pid(-1), headersParsed(false)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw(Disconnect("[CLIENT-" + _toString(clientSocket) + "] pipe: " + strerror(errno)));
	// if (data->isEncoded)
	// {
	// 	bodyFile.open(/*opened file by the request*/);
	// 	if (!bodyFile.is_open())
	// 		throw(500);
	// 	close(pipe_fd[0]);
	// }
	// else
		pipe_in = pipe_fd[0];

	pipe_out = pipe_fd[1];
	CGIreader = &CGIHandler::readCGILength;
}

int CGIHandler::getOutfd() const
{
	return (pipe_out);
}

int CGIHandler::getInfd() const
{
	return (pipe_in);
}

pid_t CGIHandler::getPid() const
{
	return (pid);
}

void	CGIHandler::storeBody()
{
	int		bytesWritten = write(pipe_out, buffer.c_str(), SEND_BUFFER_SIZE);
	if (bytesWritten == -1)
		throw(500);
	buffer.erase(bytesWritten);
}

void	CGIHandler::POSTbody(char *buf, ssize_t size)
{
	buffer.assign(buf, size);
	//process body
	HTTPserver->updateHandler(socket, 0);
	HTTPserver->updateHandler(pipe_out, EPOLLOUT);
}

void	CGIHandler::readCGIChunked()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(pipe_in, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));
	
	std::cout << YELLOW << "======[READ DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
	buffer = buildChunk(buf, bytesRead);
	state = WRITE;
	if (bytesRead == 0)
		nextState = DONE;
}

void		CGIHandler::readCGILength()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(pipe_in, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));
	}
	else if (bytesRead > 0)
	{
		buffer = std::string(buf, bytesRead);
		std::cout << YELLOW << "======[READ DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
		if (headersParsed)
			state = WRITE;
	}
	else
	{
		state = DONE;
	}
}
// void printState(State state, std::string name)
// {
// 	switch (state)
// 	{
// 		case HEADERS:
// 			std::cout << name << "===> HEADERS" << std::endl;
// 			break;
// 		case READ:
// 			std::cout << name << "===> READ" << std::endl;
// 			break;
// 		case WRITE:
// 			std::cout << name << "===> WRITE" << std::endl;
// 			break;
// 		case DONE:
// 			std::cout << name << "===> DONE" << std::endl;
// 			break;
// 	}
// }
int		CGIHandler::respond()
{
	switch (state)
	{
		case HEADERS:
			generateHeaders();
			break;
		case READ:
			HTTPserver->updateHandler(socket, 0);
			HTTPserver->updateHandler(pipe_in, EPOLLIN);
			break;
		case WRITE:
			if ((this->*sender)() == true)
				state = nextState;
			break;
		case DONE:
			return (1);
	}
	return (0);
}

void	CGIHandler::handleEvent(uint32_t events)
{
	if ((events & EPOLLIN) || ((events & EPOLLHUP) && state != DONE)) // care EPOLLHUP
	{
		(this->*CGIreader)();
		HTTPserver->updateHandler(socket, EPOLLOUT);
		HTTPserver->updateHandler(pipe_in, 0);
	}
	else if (events& EPOLLOUT)
	{
		storeBody();
		HTTPserver->updateHandler(socket, EPOLLIN);
		HTTPserver->updateHandler(pipe_out, 0);
	}
}