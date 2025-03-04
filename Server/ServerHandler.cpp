#include "ServerHandler.hpp"

ServerHandler::~ServerHandler()
{
	HTTPserver->removeHandler(socket);
}

ServerHandler::ServerHandler() {}

ServerHandler::ServerHandler(int fd) : socket(fd) {}

int		ServerHandler::getFd() const
{
	return (socket);
}

void	ServerHandler::addVServer(ServerConfig& server)
{
	vServers.push_back(server);
}

void	ServerHandler::handleEvent(uint32_t events)
{
	if (events & EPOLLIN)
	{
		int	clientSocket = accept(socket, NULL, NULL);
		if (clientSocket == -1)
		{
			std::cerr << "[WEBSERV][ERROR]\t";
			perror("accept");
			return;
		}

		if (fcntl(clientSocket, F_SETFD, FD_CLOEXEC) == -1)
		{
			std::cerr << "[WEBSERV][ERROR]\tfcntl :" << strerror(errno) << std::endl;
			close(clientSocket);
			return;
		}

		ClientHandler	*client = new ClientHandler(clientSocket, this->vServers);
		std::cout << "CREATED CLIENT: " << clientSocket << ", Pointer: " << client << std::endl;
		HTTPserver->registerHandler(clientSocket, client, EPOLLIN);
		HTTPserver->addTimer(clientSocket);

		std::cout << CYAN << "[WEBSERV][SERVER]\tClient Connected To Socket " << clientSocket << RESET << std::endl;
	}
	else if (events & EPOLLHUP)
	{
		throw(Disconnect("[SERVER-" + _toString(socket) + "] SHUTDOWN"));
	}
}