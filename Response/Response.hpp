#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <sys/socket.h>
# include <sys/stat.h>
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
	void		generateResponse( void );
	void		errorResponse();
	bool		formPath( void );

	int			sendResponse( int& socket );


	// void			feedResponse(Request* _Request);
	// void		    setMatchingLocationBlock();
	// void		    findExactMatchingLocationBlock();
	// void		    findLongestMatchingPrefixLocationBlock();
	// bool		    locationHasRedirection();
	// bool		    isMethodAllowed();

	// void		    handleGET();
	
	// void		    handleFileResource();
	// void		    handleDirectoryResource();
	// bool		    directoryContainsIndexFile();
	// void		    setRequestedResource();
	// std::string	    getRequestedResourceType();

	

	// void		    setRequest(Request* _Request);
	// void			setResponsibleConfig(std::vector<ServerConfig>& vServerConfigs);
	
private:
	// response needed data
	struct ResponseInput	input;

	// response created data
	std::map<std::string, std::string>	mimeTypes;
	std::map<int, std::string>			errorCodes;
	bool										keepAlive;

	// response creating process
	std::ifstream	bodyFile;
	bool			headersSent;
	bool			bodySent;
	int				headersOffset;
	size_t			contentLength;

	// response
	std::string	headers;
	std::string	body;

	// std::string					requestedResource;

	// int							statusCode;
	// Request*					_Request;
	// ServerConfig				_Config;
	// LocationConfig				*locationBlock;
	

};

#endif