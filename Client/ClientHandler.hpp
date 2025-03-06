#ifndef CLIENTHANDLER_HPP
# define CLIENTHANDLER_HPP

# include "../IEventHandler.hpp"
# include "Response/Response.hpp"
# include "Request/Request.hpp"
# include <iostream>

class ClientHandler : public EventHandler
{
public:
	~ClientHandler();
	ClientHandler(int fd, std::vector<ServerConfig>& vServers);


	void	reset();
	void 	handleRead();
	void 	handleWrite();
	void	handleEvent(uint32_t events);

	void	createResponse();
	void	deleteResponse();
	
	int		getFd() const;
	bool	getCgiActive() const;

private:
	int							socket;

	bool						cgiActive;
	AResponse					*response;
	Request						request;

	std::vector<ServerConfig>&	vServers;
	Directives					config;
	
	e_reqType					reqState;
};

#endif