#ifndef RES_HPP
# define RES_HPP

# include <iostream>
# include <vector>
# include <map>
# include "../../_Config/Config.hpp"
# include <dirent.h>
# include <cstring>
# include <sys/socket.h>
# include <algorithm>
# include "../Request/Request.hpp"

# define SEND_BUFFER_SIZE 4096

enum e_RangeState
{
	NEXT,
	GET
};

typedef	struct								Range
{
	std::pair<int, int>						range;
	std::string								header;
	size_t									rangeLength;
	bool									headerSent;
	Range() : headerSent(false) {}
}											Range;

typedef struct								RangeData
{
	std::vector<Range>						ranges;
	std::vector<Range>::iterator			current;
	std::string								boundary;
	e_RangeState							rangeState;
	RangeData() : rangeState(NEXT) {}
}											RangeData;

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
	virtual ~Response();
	Response();
	Response(int &clientSocket, RequestData *data);
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

	void	generateErrorPage(int& status);
	void	generateHeaders( void );
	bool	sendHeaders();
	bool	sendBody();


	virtual int	respond();
	
protected:
	int				socket;
	enum State		state;
	enum State		nextState;
	std::string		headers;
	std::string		buffer;
	bool			(Response::*sender)();
	void			(Response::*reader)();
	RequestData		*reqCtx;
	std::ifstream	bodyFile;
	std::map<int, std::string>			statusCodes;

private:

	void	directoryListing();
	void	parseRangeHeader();
	int		rangeContentLength( void );
	void	handleRange();
	void	handlePOST( void );
	void	handleGET( void );

	struct RangeData	rangeData;
	DIR					*dirList;

	std::map<std::string, std::string>	mimeTypes;

	std::string		contentType;
	unsigned long	contentLength;

};

#endif