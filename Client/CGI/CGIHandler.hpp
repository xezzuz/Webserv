#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "../../IEventHandler.hpp"
# include "../Response/Response.hpp"

# define CGI_BUFFER_SIZE 4096

enum CGIState
{
	PARSE,
	BRIDGE
};

class CGIHandler : public EventHandler, public Response
{
public:
	~CGIHandler();
	CGIHandler(int& clientSocket);

	int		setup();
	// void	readCgi();
	void	handleEvent(uint32_t events);
	



	// int		getFd() const;
	pid_t	getPid() const;
	int		getFd() const
	{
		return (outfd);
	}

	std::vector<std::string>	headersToEnv();
	char						**buildEnv();

	void	parseCGIHeaders();
	void	generateCGIHeaders();

	void	readCgi();
	int	feedCgi(const char *buf);


private:
	// created here
	int				outfd; // file descriptor where cgi writes its output into
	int				infd; // file descriptor where cgi reads input from
	pid_t			pid;
	enum CGIState	CGIState;
};

#endif