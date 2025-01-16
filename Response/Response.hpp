#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <sys/socket.h>
# include <sys/stat.h>
# include <cstdio>
# include <algorithm>
# include <queue>
# include <cstdlib>
# include "../Config/Config.hpp"
# include "../Utils/Helpers.hpp"

struct Range
{
	std::pair<int, int> range;
	std::string			header;
	bool				headerSent = false;
};

enum	ResponseState
{
	SENDINGHEADER,
	SENDINGBODY,
	SENDINGRANGES,
	SENDINGENDMARK,
	ERROR,
	FINISHED
};

# define SEND_BUFFER_SIZE 4096
// enum e_responseState {
// 	PARSING_INIT,		// 0
// 	HEADERS_RECEIVED,	// 1
// 	HEADERS_FINISHED,	// 2
// 	BODY_RECEIVED,		// 3
// 	BODY_FINISHED,		// 4
// 	PARSING_FINISHED	// 5
// };

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
	bool		removeResource( void );
	bool		validateUri( void );
	bool		getResource( void );


	int			rangeContentLength( void );

	bool		parseRangeHeader( void );
	bool		buildRange( void );



	int			sendResponse( int& socket );
	void		sendRanges( int& socket );
	void		sendBody( int& socket );
	void		sendHeader( int& socket );
	void		sendEndMark( int& socket );
	
private:
	// response needed data
	struct ResponseInput	input;

	// response created data
	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;


	// response frequent uses
	std::string							contentType;
	long								contentLength;


	// response creating process
	std::string		headers;
	std::string		body;
	std::ifstream	bodyFile;
	int				headersOffset;
	std::string		absolutePath;
	bool			isDir; // requested resource is a directory;

	// range
	std::vector<Range>	ranges;
	std::string			boundary;
	std::string			endMark;
	int					currRange;
	int					rangeOffset;

	enum ResponseState					state;

	// response
	
};

#endif