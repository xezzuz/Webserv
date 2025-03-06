#include "Webserv.hpp"
#include "../Server/ServerHandler.hpp"

bool	Webserv::running = true;

Webserv::~Webserv()
{
	while (!handlerMap.empty())
		cleanup(handlerMap.begin()->second);
	close(epoll_fd);
}

Webserv::Webserv(std::vector<ServerConfig>& servers) : servers(servers)
{
	srand(std::time(0));
	epoll_fd = epoll_create1(0);
}

void Webserv::stop()
{
	running = false;
}

void	Webserv::addTimer(int fd)
{
	clientTimer.push_back(std::make_pair(fd, std::time(NULL)));
}

void	Webserv::updateTimer(int fd)
{
	std::vector<std::pair<int, time_t> >::iterator it;
	for (it = clientTimer.begin(); it != clientTimer.end(); it++)
	{
		if (it->first == fd)
		{
			it->second = std::time(NULL);
			break;
		}
	}
}

void	Webserv::eraseTimer(int fd)
{
	std::vector<std::pair<int, time_t> >::iterator it;
	for (it = clientTimer.begin(); it != clientTimer.end(); it++)
	{
		if (it->first == fd)
		{
			clientTimer.erase(it);
			break;
		}
	}
}

void	Webserv::collect(EventHandler *handler)
{
	deleted.push_back(handler);
}

void	Webserv::registerDependency(EventHandler *dependent, EventHandler *dependency)
{
	dependencyMap[dependent] = dependency;
}

void	Webserv::registerHandler(int fd, EventHandler *handler, uint32_t events)
{
	struct epoll_event ev;

	ev.events = events;
	ev.data.ptr = handler;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
	handlerMap.insert(std::make_pair(fd, handler));
	handler->HTTPserver = this;
}

void	Webserv::updateHandler(const int fd, uint32_t events)
{
	struct epoll_event ev;

	ev.events = events;
	ev.data.ptr = handlerMap[fd];
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void	Webserv::removeHandler(int fd)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	std::map<int, EventHandler*>::iterator it = handlerMap.find(fd);
	if (it != handlerMap.end())
	{
		handlerMap.erase(it);
	}
	close(fd);
}

int	Webserv::bindSocket(std::string& host, std::string& port)
{
	struct addrinfo		hints;
	
	// initialize addrinfo
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo	*res;
	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
	if (status != 0)
	{
		std::cerr << YELLOW << "\tWebserv: getaddrinfo: " << gai_strerror(status) << RESET << std::endl;
		exit(errno);
	}

	// create socket and assign (bind) that socket an address returned in res
	struct addrinfo		*it;
	int					serverSocket = -1;

	for (it = res; it; it = it->ai_next)
	{
		serverSocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
		if (serverSocket == -1)
			continue;

		// kernel has a wait period for ports to be reusable after a socket has been closed under normal behaviour
		// setsocketopt specifically the 3rd argument SO_REUSEADDR and SO_REUSEPORT allows the address port to be reusable
		// in our case we just prepare the socket to be able to bind to an address that already in use
		// that would be if the server crashed and restarted the previous address bound to the socket wouldn't be unavaible due to the kernel wait period 

		int yes = 1;
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) == -1)
		{
			std::cerr << YELLOW << "\tWebserv: setsocketopt: " << strerror(errno) << RESET << std::endl;
			close(serverSocket);
			freeaddrinfo(res);
			exit(errno);
		}

		if (bind(serverSocket, it->ai_addr, it->ai_addrlen) == 0)
			break;
		close(serverSocket);
	}
	freeaddrinfo(res);
	if (!it)
	{
		close(serverSocket);
		std::cerr << YELLOW << "\tWebserv : Failed to Bind Any Socket..." << RESET << std::endl;
		exit(errno);
	}
	return (serverSocket);
}

void    Webserv::listenForConnections(int& serverSocket)
{
	if (listen(serverSocket, BACKLOG) == -1)
	{
		std::cerr << YELLOW << "\tWebserv : listen: " << strerror(errno) << RESET << std::endl;
		close(serverSocket);
		exit(errno);
	}
	if (fcntl(serverSocket, F_SETFD, FD_CLOEXEC) == -1) // sets the socket to nonblock mode so it doesn't "block" on I/O operations (accept(), recv() ..)
	{
		std::cerr << YELLOW << "\tWebserv : fcntl: " << strerror(errno) << RESET << std::endl;
		close(serverSocket);
		exit(errno);
	}
}

void	Webserv::initServers()
{
	int													serverSocket;
	std::vector<ServerConfig>::iterator					it;
	std::map<std::pair<std::string, std::string>, int>	boundServers;

	for (it = servers.begin(); it != servers.end(); it++)
	{
		std::pair<std::string, std::string> bindAddress = std::make_pair(it->host, it->port);
		std::map<std::pair<std::string, std::string>, int>::iterator deja = boundServers.find(bindAddress);

		// if server already bound just add the virtual server config to the ServerHandler
		if (deja != boundServers.end())
		{
			static_cast<ServerHandler *>(handlerMap[deja->second])->addVServer(*it);
			continue ;
		}

		// resolves domain name bind serverSocket to sockaddr and returns a valid socket
		serverSocket = bindSocket(it->host, it->port);
		listenForConnections(serverSocket);
		std::cout << BLUE << BOLD << "Server listening on " << it->host << ":" << it->port << RESET << std::endl;

		ServerHandler	*handler = new ServerHandler(serverSocket);
		handler->addVServer(*it);
		boundServers.insert(std::make_pair(bindAddress, serverSocket));
		registerHandler(serverSocket, handler, EPOLLIN);
	}
}

void	Webserv::clientTimeout()
{
	time_t now = std::time(NULL);

	for (timeIt = clientTimer.begin(); timeIt != clientTimer.end(); )
	{
		if (now - timeIt->second >= TIMEOUT)
		{
			std::map<int, EventHandler*>::iterator clientIt = handlerMap.find(timeIt->first);
			int clientFd = timeIt->first;

			timeIt = clientTimer.erase(timeIt);

			if (clientIt != handlerMap.end())
				delete clientIt->second;
			else
				close(clientFd);
		}
		else
			++timeIt;
	}
}

void	Webserv::cleanup(EventHandler *handler)
{
	if (!handler)
		return;

	std::map<EventHandler *, EventHandler *>::iterator it;
	it = dependencyMap.find(handler);
	if (it != dependencyMap.end())
	{
		EventHandler	*dependency = it->second;
		dependencyMap.erase(it);
		delete dependency; // deleting the dependency deletes the dependent in the destructor
	}
	else
		delete handler;
}

void	Webserv::run()
{
	struct epoll_event events[MAX_EVENTS];
	while (running)
	{
		int eventCount = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);

		for (int i = 0; i < eventCount; i++)
		{
			EventHandler	*handler = static_cast<EventHandler *>(events[i].data.ptr);
			if (std::find(deleted.begin(), deleted.end(), handler) != deleted.end())
				continue;

			try
			{
				if (events[i].events & EPOLLERR)
				{
					std::cerr << RED << "\tWebserv : error on socket " << handler->getFd() << RESET << std::endl;
					cleanup(handler);
				}
				else
					handler->handleEvent(events[i].events);
			}
			catch (Disconnect& e)
			{
				std::cerr << YELLOW << e.what() << RESET << std::endl;
				cleanup(handler);
			}
		}
		deleted.clear();
		clientTimeout();
	}
}
