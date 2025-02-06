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
# include "Error.hpp"
# include "../Request/Request.hpp"

# define SEND_BUFFER_SIZE 4096

enum State
{
	READ,
	WRITE,
	DONE
};

enum RangeState
{
	NEXT,
	GET
};

struct Range
{
	std::pair<int, int> range;
	std::string			header;
	size_t				rangeLength;
	bool				headerSent;
	Range() : headerSent(false) {}
};

struct RangeData
{
	std::vector<Range>				ranges;
	std::vector<Range>::iterator	current;
	std::string						boundary;
	enum RangeState					rangeState;
};

class Response
{
public:
	~Response();
	Response();
	Response(int &clientSocket);

	// this is for stack Response
	void	setPath(const std::string& path);
	void	setFunc(const enum Operation& op);
	void	setRange(const RangeData& data);
	void	setSocket(int& clientSocket);
	void	setContext(struct RequestData *ctx);



	std::string	buildChunk(const char *data, size_t size);
	void		initDirList();


	bool	sendHeaders();
	bool	sendBody();
	int		respond();
	
protected:
	enum State		state;
	enum State		nextState;
	std::string		headers;
	std::string		buffer;
	bool			(Response::*sender)();
	void			(Response::*reader)();
	int				socket;
	RequestData		*reqCtx;

private:

	void	directoryListing();
	void	readBody();
	void	nextRange();
	void	readRange();
	void	range();
	int		rangeContentLength( void );
	void	handleRange();
	void	handlePOST( void );
	void	handleGET( void );
	void	generateHeaders( void );

	std::ifstream	bodyFile;
	DIR				*dirList;

	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;

	std::string		contentType;
	unsigned long	contentLength;

};

#endif