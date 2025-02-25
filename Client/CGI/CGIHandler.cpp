#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"

CGIHandler::~CGIHandler()
{
	HTTPserver->removeHandler(pipe_in);
	HTTPserver->removeHandler(pipe_out);

	if (waitpid(pid, NULL, WNOHANG) == 0)
		kill(pid, SIGTERM);
}

CGIHandler::CGIHandler(int& clientSocket, RequestData *data) : AResponse(clientSocket, data), bodySize(0), pid(-1), headersParsed(false)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "[WEBSERV][ERROR]\tpipe: " << strerror(errno) << std::endl;
		throw(500);
	}
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

bool	CGIHandler::storeBody()
{
	int		bytesWritten = write(pipe_out, buffer.c_str(), buffer.size());
	if (bytesWritten == -1)
		throw(500); // big problem
	std::cout << "PIP_OUT STORE_____" << std::endl << buffer;
	std::cout << "BUFFERSIZE: " << buffer.size() << std::endl;
	std::cout << "BytESwRittne: " << bytesWritten << std::endl;
	std::cout << "_____PIP_OUT STORE" << std::endl;
	buffer.erase(0, bytesWritten);
	return (buffer.empty());
}

void	CGIHandler::setBuffer(std::string buffer)
{
	std::cout << "SETBUFFER(string) SIZE: " << buffer.size() << std::endl;
	if (!buffer.empty())
	{
		this->buffer = buffer;
		bodySize += buffer.size();
		HTTPserver->updateHandler(socket, 0);
		HTTPserver->updateHandler(pipe_out, EPOLLOUT);
	}
}

void	CGIHandler::setBuffer(char *buf, ssize_t size)
{
	std::cout << "SETBUFFER(char) SIZE: " << size << std::endl;
	if (size)
	{
		buffer = std::string(buf, size);
		bodySize += size;
		HTTPserver->updateHandler(socket, 0);
		HTTPserver->updateHandler(pipe_out, EPOLLOUT);
	}
}

void	CGIHandler::readCGIChunked()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(pipe_in, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));

	std::cout << YELLOW << "======[READ(CHUNKED) DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
	buffer = buildChunk(buf, bytesRead);
	std::cout << buffer;
	std::cout << "=======================================" << std::endl;
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
		std::cout << YELLOW << "======[READ(LENGTH) DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
		std::cout << buffer;
		std::cout << "=======================================" << std::endl;
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
		std::cout << "CGIREADER BUFFER::::::::::::::::::::::::::::::::::" <<std::endl << buffer;
		std::cout << "::::::::::::::::::::::::::::::::::CGIREADER BUFFER" << std::endl;;
		HTTPserver->updateHandler(socket, EPOLLOUT);
		HTTPserver->updateHandler(pipe_in, 0);
	}
	else if (events& EPOLLOUT)
	{
		if (storeBody() == true)
		{
			std::cout << "BODYSIZE: " << bodySize << "|:|:|:|CONTENT-LENGTH: " << reqCtx->contentLength << std::endl;
			if (bodySize == reqCtx->contentLength)
			{
				std::cout << "STORE_BODY_CHILD_EPOLLIN" << std::endl;
				HTTPserver->updateHandler(socket, 0);
				HTTPserver->updateHandler(pipe_out, 0);
				HTTPserver->updateHandler(pipe_in, EPOLLIN);
			}
			else if (bodySize > reqCtx->contentLength)
				throw(400); // problem
			else
			{
				std::cout << "STORE_BODY_SOCKET_EPOLLIN" << std::endl;
				HTTPserver->updateHandler(socket, EPOLLIN);
				HTTPserver->updateHandler(pipe_in, 0);
				HTTPserver->updateHandler(pipe_out, 0);
			}
		}
	}
}