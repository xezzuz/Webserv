#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "../../IEventHandler.hpp"
# include "../Response/AResponse.hpp"

class CGIHandler : public EventHandler, public AResponse
{
public:
	virtual ~CGIHandler();
	CGIHandler(int& clientSocket, RequestData *data);
	CGIHandler(const CGIHandler& rhs);
	CGIHandler& operator=(const CGIHandler& rhs);
	
	int		getInfd() const;
	int		getOutfd() const;
	pid_t	getPid() const;
	int		getFd() const
	{
		return (pipe_in);
	}
	std::vector<std::string>	headersToEnv();
	void						buildEnv();


	void	execCGI();
	void	handleEvent(uint32_t events);

	void	validateHeaders();
	void	parseHeaders();
	void	addHeaders();
	void	generateHeaders();

	
	void	readCGILength();
	void	readCGIChunked();
	void	storeBody();
	void	POSTbody(char *buf, ssize_t size);


	int		respond();

private:
	// created here
	std::vector<std::string>			envVars;
	std::vector<char *>					envPtr;
	char								*args[3];
	std::map<std::string, std::string>	headersMap;


	size_t			inputProcessed;
	int				pipe_in; // file descriptor where cgi reads input from
	int				pipe_out; // file descriptor where cgi writes its output into
	std::ifstream	bodyFile; // file to store incoming client body in if the request is chunked
	pid_t			pid;
	bool			headersParsed;
	void			(CGIHandler::*CGIreader)();
};

#endif