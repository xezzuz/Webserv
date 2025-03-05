#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../IEventHandler.hpp"
#include "../_Config/Config.hpp"
#include <algorithm>
#include <netdb.h>
#include <map>
#include <errno.h>
#include <vector>
#include <ctime>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

# define MAX_EVENTS 100
# define BACKLOG 128
# define TIMEOUT 5
# define TIMEOUT_MS 5000

class Webserv
{
public:
	~Webserv();
	Webserv(std::vector<ServerConfig>& servers);

	void	registerDependency(EventHandler *dependent, EventHandler *dependency);
	void	registerHandler(const int fd, EventHandler *h, uint32_t events);
	void	updateHandler(const int fd, uint32_t events);
	void	removeHandler(const int fd);

	void	addTimer(int fd);
	void	updateTimer(int fd);
	void	eraseTimer(int fd);
	void	clientTimeout();

	void	collect(EventHandler *handler);
	void	cleanup(EventHandler *handler);

	void		initServers();
	static void	stop();
	void		run();

private:
	int 	bindSocket(std::string& host, std::string& port);
	void    listenForConnections(int& listener);

	static bool						running;

	int								epoll_fd;
	std::vector<ServerConfig>		servers;

	std::map<int, EventHandler *>				handlerMap;
	std::map<EventHandler *, EventHandler *>	dependencyMap;

	std::list<EventHandler *>					deleted;

	std::vector<std::pair<int, time_t> >			clientTimer;
	std::vector<std::pair<int, time_t> >::iterator	timeIt;
};

#endif