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


	int	Response::rangeContentLenght( void );

	bool		parseRangeHeader( void );
	bool		buildRange( void );



	int			sendResponse( int& socket );
	int			sendRanges( int& socket );
	
private:
	// response needed data
	struct ResponseInput	input;

	// response created data
	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			statusCodes;
	bool								keepAlive;

	// response creating process
	std::ifstream						bodyFile;
	bool								headersSent;
	bool								bodySent;
	int									headersOffset;
	std::string							contentType;
	size_t								contentLength;
	bool								isDir; // request uri is a directory;

	// range
	std::pair<int, int>	range;


	// response
	std::string		headers;
	std::string		body;
	
};

#endif