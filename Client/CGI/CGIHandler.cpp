#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"
#include "../Response/Error.hpp"

CGIHandler::~CGIHandler()
{
	close(infd);
	close(outfd);
	if (waitpid(pid, NULL, WNOHANG) == 0)
		kill(pid, SIGTERM);
}

CGIHandler::CGIHandler(int& clientSocket, RequestData *data) : Response(clientSocket, data), infd(-1), outfd(-1), pid(0), parseBool(true), chunked(false)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw(Disconnect("[CLIENT-" + _toString(clientSocket) + "] pipe: " + strerror(errno)));
	infd = pipe_fd[0];
	outfd = pipe_fd[1];
	// state = PARSE;
}

pid_t CGIHandler::getPid() const
{
	return (pid);
}

void	CGIHandler::storeBody()
{
	int		bytesWritten = write(infd, buffer.c_str(), SEND_BUFFER_SIZE);
	if (bytesWritten == -1)
		throw(500);
	buffer.erase(bytesWritten);
}

void	CGIHandler::receiveCGIInput()
{
	char	body[RECV_BUFFER_SIZE];
	ssize_t	bytesRead = recv(socket, body, RECV_BUFFER_SIZE, 0);
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] recv: " + strerror(errno)));
	}
	else if (bytesRead > 0)
	{
		buffer.append(std::string(body, bytesRead));
		// process body
		// if body finished next state = done
		state = WRITE;
	}
	else
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	}
}

void		CGIHandler::readCGIOutput()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(outfd, buf, SEND_BUFFER_SIZE);
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));
	}
	if (bytesRead > 0)
	{
		buffer = std::string(buf, bytesRead);
		std::cout << YELLOW << "======[READ DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
		state = WRITE;
	}
	else
	{
		state = DONE;
	}
}

int	CGIHandler::receive( void )
{
	switch (state)
	{
		case READ:
			receiveCGIInput();
			break;
		case WRITE:
			HTTPserver->updateHandler(socket, EPOLLHUP);
			HTTPserver->updateHandler(infd, EPOLLOUT | EPOLLHUP);
			break;
		case DONE:
			return (1);
	}
	return (0);
}

int		CGIHandler::respond()
{
	switch (state)
	{
		case READ:
			HTTPserver->updateHandler(socket, EPOLLHUP);
			HTTPserver->updateHandler(outfd, EPOLLIN | EPOLLHUP);
			break;
		case WRITE:
			if (parseBool)
				parseCGIHeaders();
			if ((this->*sender)() == true)
			{
				if (buffer.empty())
					state = nextState;
				else if (chunked)
					buffer = buildChunk(buffer.c_str(), buffer.size());
			}
			break;
		case DONE:
			return (1);
	}
	return (0);
}

void	CGIHandler::handleEvent(uint32_t events)
{
	if ((events & EPOLLIN || events & EPOLLHUP) && state != DONE) // care EPOLLHUP
	{
		readCGIOutput();
		if (chunked)
			buffer = buildChunk(buffer.c_str(), buffer.size());

		HTTPserver->updateHandler(socket, EPOLLOUT | EPOLLHUP);
		HTTPserver->updateHandler(outfd, 0);
	}
	else if (events& EPOLLOUT)
	{
		storeBody();
		HTTPserver->updateHandler(socket, EPOLLIN | EPOLLHUP);
		HTTPserver->updateHandler(infd, 0);
	}
}