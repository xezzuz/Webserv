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
	LISTDIR,
	NEXTRANGE,
	READRANGE,
	SENDDATA,
	ERROR,
	FINISHED
};

struct	ResponseInput
{
	std::string							uri;
	std::string							method;
	int									status;
	std::map<std::string, std::string>	requestHeaders;
	Directives							config;
};

# define SEND_BUFFER_SIZE 4096

class Response
{
public:
	Response();
	Response(const Response& rhs);
	Response&	operator=(const Response& rhs);
	~Response();

	void		setInput(struct ResponseInput& input);

	void		generateErrorPage( void );
	void		generatePostPage( void );
	void		generateResponse( void );
	bool		validateUri( void );
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
	void		getNextRange();
	bool		sendData(int& socket);
	
	void		directoryListing();



private:
	// response needed data
	struct ResponseInput	input;

	// response created data
	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;


	// response frequent uses
	std::string		contentType;
	long			contentLength;


	// response creating process
	std::string		headers;
	std::ifstream	bodyFile;
	std::string		absolutePath;
	bool			isDir; // requested resource is a directory;

	DIR				*dirList;

	// range
	std::vector<Range>	ranges;
	std::string			boundary;
	size_t				currRange;

	enum State	state;
	enum State	nextState;

	// response
	std::string data;
	


};

#endif