#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <sys/socket.h>
# include <sys/stat.h>
# include <cstdio>
# include <algorithm>
# include <dirent.h>
# include <cstdlib>
# include "../Config/Config.hpp"
# include "../Utils/Helpers.hpp"

# define SEND_BUFFER_SIZE 4096

struct Range
{
	std::pair<int, int> range;
	std::string			header;
	size_t				rangeLength;
	bool				headerSent = false;
};

enum	State
{
	BUILDHEADER,
	READBODY,
	AUTOINDEX,
	LISTDIR,
	BUILDCHUNK,
	NEXTRANGE,
	READRANGE,
	SENDDATA,
	ERROR,
	FINISHED
};

struct	ResponseInput
{
	std::string							method;
	std::string							uri;
	std::string							absolutePath;
	bool								isDir = false; // requested resource is a directory;
	int									status;
	std::map<std::string, std::string>	requestHeaders;
	Directives							config;
};

class Response
{
public:
	~Response();
	Response();
	Response(const Response& rhs);
	Response&	operator=(const Response& rhs);

	void		setInput(struct ResponseInput& input);

	void		generateErrorPage( void );
	void		generateHeaders( void );
	bool		getResource( void );


	int			rangeContentLength( void );

	bool		parseRangeHeader( void );
	void		buildRange( void );


	void		handleGET( void );
	void		handlePOST( void );
	void		handleDELETE( void );


	void		readBody();
	int			sendResponse( int& socket );
	void		readRange();
	void		buildChunk();
	void		getNextRange();
	bool		sendData(int& socket);
	
	void		autoIndex();
	void		directoryListing();



private:
	// response needed data
	struct ResponseInput	input;

	// response constants
	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;


	// response frequent uses
	std::string		contentType;
	unsigned long	contentLength;


	// response creating process
	std::string		headers;
	std::ifstream	bodyFile;
	bool			chunked;
	DIR				*dirList; // might produce leaks

	// range
	std::vector<Range>	ranges;
	std::string			boundary;
	size_t				currRange;

	// response state
	enum State	state;
	enum State	nextState;

	// response buffer
	std::string data;
};

#endif