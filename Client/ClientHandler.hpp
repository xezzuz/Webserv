#ifndef CLIENTHANDLER_HPP
# define CLIENTHANDLER_HPP

# include "../IEventHandler.hpp"
# include "Response/Response.hpp"
# include "Response/Error.hpp"
# include "Request/Request.hpp"
# include <iostream>

enum e_bridgeState {
	HEADERS = 1,
	BODY = 2,
};

class ClientHandler : public EventHandler
{
public:
	~ClientHandler();
	ClientHandler(int fd, std::vector<ServerConfig>& vServers);


	ServerConfig&	matchingServer(std::string& host);
	void			decodeUri(struct ResponseInput& input, std::string& URL);
	void			initResponse();
	void			setupResponse();
	int				getSocket( void ) const;
	// void			setResponseBuffer(std::string buffer);


	void	receive();

	void	reset();
	void	remove();
	void 	handleRead();
	void 	handleWrite();
	void	handleEvent(uint32_t events);

	void	createResponse();
	void	deleteResponse();
	
	int		getFd() const
	{
		return (socket);
	}

private:
	int							socket;
	Request						request;

	Response					*response;

	std::vector<ServerConfig>&	vServers;
	Directives					config;
	
	bool						cgiActive;
	bool						keepAlive;
	e_bridgeState				bridgeState;
	
};

void			fillRequestData(const std::string URI, RequestData& _RequestData);
void			decodeAbsPath(const std::string ABSPATH, RequestData& _RequestData);

#endif