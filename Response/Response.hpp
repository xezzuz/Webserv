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
	std::string							queryString;
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

	// external functions
	void		setInput(struct ResponseInput& input);
	int			sendResponse( int& socket );

	void		generateErrorPage( void );
	void		generateHeaders( void );
	bool		getResource( void );

	// range parsing
	int			rangeContentLength( void );
	bool		parseRangeHeader( void );



	void		handleGET( void );
	void		handlePOST( void );
	void		handleDELETE( void );

	// sending body
	void		readBody();
	void		readRange();
	void		buildRange( void );
	void		buildChunk();
	void		getNextRange();
	bool		sendData(int& socket);
	
	// autoindex
	void		autoIndex();
	void		directoryListing();

	// CGI



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