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

enum RangeState
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
	RangeState								rangeState;
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

	void setBuffer(const std::string& other);

	inline std::string	buildChunk(const char *data, size_t size) // error
	{
		return (toHex(size) + "\r\n" + std::string(data, size) + "\r\n");
	}


	virtual void	POSTbody(char *buf, ssize_t size);
	virtual void	storeBody( void );

	void			generateErrorPage(int& status);
	void			generateHeaders( void );
	bool			sendHeaders();
	virtual bool	sendBody();
	virtual int		respond();
	
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
	std::ofstream	uploadFile;
	std::map<int, std::string>			statusCodes;

private:
	void	handlePOST();

	int		rangeContentLength( void );
	void	parseRangeHeader();
	void	nextRange();
	void	readRange();
	void	range();
	void	readBody();

	void	directoryListing();
	void	initDirList();
	
	void	handleRange();
	void	handleGET( void );

	struct RangeData	rangeData;
	DIR					*dirList;

	std::map<std::string, std::string>	mimeTypes;

	std::string		contentType;
	unsigned long	contentLength;

};

#endif