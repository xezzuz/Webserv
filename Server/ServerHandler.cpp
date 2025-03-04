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

		struct linger so_linger;

		so_linger.l_onoff = 1; // enable linger option
		so_linger.l_linger = 5; // seconds system will wait before closing the fd after closed is called
		if (setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(so_linger)) == -1)
		{
			std::cerr << "[WEBSERV][ERROR]\tsetsocketopt: " << strerror(errno) << std::endl;
			close(clientSocket);
			return;
		}

		if (fcntl(clientSocket, F_SETFD, FD_CLOEXEC) == -1)
		{
			std::cerr << "[WEBSERV][ERROR]\tfcntl :" << strerror(errno) << std::endl;
			close(clientSocket);
			return;
		}

		ClientHandler	*client = new ClientHandler(clientSocket, this->vServers);
		HTTPserver->registerHandler(clientSocket, client, EPOLLIN);
		HTTPserver->addTimer(clientSocket);

		std::cout << CYAN << "[WEBSERV][SERVER]\tClient Connected To Socket " << clientSocket << RESET << std::endl;
	}
	else if (events & EPOLLHUP)
	{
		throw(Disconnect("[SERVER-" + _toString(socket) + "] SHUTDOWN"));
	}
}