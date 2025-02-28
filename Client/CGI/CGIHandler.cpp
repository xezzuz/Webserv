#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"

CGIHandler::~CGIHandler()
{
	HTTPserver->removeHandler(cgiSocket);

	if (waitpid(pid, NULL, WNOHANG) == 0)
		kill(pid, SIGTERM);
}

CGIHandler::CGIHandler(int& clientSocket, RequestData *data) : AResponse(clientSocket, data), cgiSocket(-1), bodySize(0), pid(-1), headersParsed(false)
{
	// if (data->isEncoded)
	// {
	// 	bodyFile.open(/*opened file by the request*/);
	// 	if (!bodyFile.is_open())
	// 		throw(500);
	// 	close(pipe_fd[0]);
	// }

	CGIreader = &CGIHandler::readCGILength;
}

int	CGIHandler::getFd() const
{
	return (cgiSocket);
}

pid_t CGIHandler::getPid() const
{
	return (pid);
}

bool	CGIHandler::storeBody()
{
	int		bytesWritten = write(cgiSocket, buffer.c_str(), buffer.size());
	if (bytesWritten == -1)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] write: " + strerror(errno)));
	std::cout << "SOCKPAIR STORE_____" << std::endl << buffer;
	std::cout << "_____SOCKPAIR STORE" << std::endl;
	buffer.erase(0, bytesWritten);
	return (buffer.empty());
}

void	CGIHandler::setBuffer(std::string buffer)
{
	if (!buffer.empty())
	{
		this->buffer = buffer;
		bodySize += buffer.size();
		HTTPserver->updateHandler(socket, 0);
		HTTPserver->updateHandler(cgiSocket, EPOLLOUT);
	}
}

void	CGIHandler::setBuffer(char *buf, ssize_t size)
{
	if (size)
	{
		buffer = std::string(buf, size);
		bodySize += size;
		HTTPserver->updateHandler(socket, 0);
		HTTPserver->updateHandler(cgiSocket, EPOLLOUT);
	}
}

void	CGIHandler::readCGIChunked()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(cgiSocket, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));

	std::cout << YELLOW << "======[READ(CHUNKED) DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
	std::cout << buffer;
	std::cout << "+++++++++++++++++++++++++" << std::endl;
	buffer = buildChunk(buf, bytesRead);
	state = WRITE;
	if (bytesRead == 0)
		nextState = DONE;
}

void		CGIHandler::readCGILength()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(cgiSocket, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));
	}
	else if (bytesRead > 0)
	{
		buffer = std::string(buf, bytesRead);
		std::cout << YELLOW << "======[READ(LENGTH) DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
		std::cout << buffer;
		std::cout << "+++++++++++++++++++++++++" << std::endl;
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
			HTTPserver->updateHandler(cgiSocket, EPOLLIN);
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
	if (events & EPOLLIN) // care EPOLLHUP
	{
		(this->*CGIreader)();
		HTTPserver->updateHandler(socket, EPOLLOUT);
		HTTPserver->updateHandler(cgiSocket, 0);
	}
	else if (events& EPOLLOUT)
	{
		if (storeBody() == true)
		{
			if (bodySize == reqCtx->contentLength)
			{
				HTTPserver->updateHandler(socket, 0);
				HTTPserver->updateHandler(cgiSocket, EPOLLIN);
			}
			else if (bodySize > reqCtx->contentLength)
				throw(400); // problem
			else
			{
				HTTPserver->updateHandler(socket, EPOLLIN);
				HTTPserver->updateHandler(cgiSocket, 0);
			}
		}
	}
}