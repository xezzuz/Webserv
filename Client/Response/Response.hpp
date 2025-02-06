#ifndef RES_HPP
# define RES_HPP

# include <iostream>
# include <vector>
# include <map>
# include "../../Config/Config.hpp"
# include <dirent.h>
# include <cstring>
# include <sys/socket.h>
# include <algorithm>
# include "../Request/Request.hpp"

# define SEND_BUFFER_SIZE 4096

enum State
{
	READ,
	WRITE,
	DONE
};

class Error;

class Response
{
public:
	~Response();
	Response();
	Response(int &clientSocket);
	Response(const Response& rhs);
	Response& operator=(const Response& rhs);

	// this is for stack Response
	// void	setPath(const std::string& path);
	// void	setFunc(const enum Operation& op);
	// void	setRange(const RangeData& data);
	void	setSocket(int& clientSocket);
	void	setContext(struct RequestData *ctx);



	std::string	buildChunk(const char *data, size_t size);
	void		initDirList();

	void	readBody();
	void	nextRange();
	void	readRange();
	void	range();

	void	generateHeaders( void );
	bool	sendHeaders();
	bool	sendBody();
	int		respond();
	
protected:
	int				socket;
	enum State		state;
	enum State		nextState;
	std::string		headers;
	std::string		buffer;
	bool			(Response::*sender)();
	void			(Response::*reader)();
	RequestData		*reqCtx;

private:

	void	directoryListing();
	int		rangeContentLength( void );
	void	handleRange();
	void	handlePOST( void );
	void	handleGET( void );

	std::ifstream	bodyFile;
	DIR				*dirList;

	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;

	std::string		contentType;
	unsigned long	contentLength;

};

#endif