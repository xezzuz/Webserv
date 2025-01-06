#include "Response.hpp"

Response::~Response() {}

Response::Response() : contentLength(0), headersSent(false), bodySent(false), headersOffset(0)
{
	components.index.push_back("default.html");
	components.index.push_back("index.html");
	components.root = "/home/mmaila/Desktop/duoSERV/www/";
    errorCodes.insert(std::make_pair(200, "OK"));
    errorCodes.insert(std::make_pair(201, "Created"));
    errorCodes.insert(std::make_pair(204, "No Content"));
    errorCodes.insert(std::make_pair(206, "Partial Content"));
    errorCodes.insert(std::make_pair(301, "Moved Permanently"));
    errorCodes.insert(std::make_pair(302, "Found"));
    errorCodes.insert(std::make_pair(303, "See Other"));
    errorCodes.insert(std::make_pair(307, "Temporary Redirect"));
    errorCodes.insert(std::make_pair(308, "Permanent Redirect"));
    errorCodes.insert(std::make_pair(400, "Bad Request"));
    errorCodes.insert(std::make_pair(403, "Forbidden"));
    errorCodes.insert(std::make_pair(404, "Not Found"));
    errorCodes.insert(std::make_pair(405, "Method Not Allowed"));
    errorCodes.insert(std::make_pair(411, "Length Required"));
    errorCodes.insert(std::make_pair(413, "Payload Too Large"));
    errorCodes.insert(std::make_pair(415, "Unsupported Media Type"));
    errorCodes.insert(std::make_pair(500, "Internal Server Error"));
    errorCodes.insert(std::make_pair(501, "Not Implemented"));
    errorCodes.insert(std::make_pair(504, "Gateway Timeout"));
    errorCodes.insert(std::make_pair(505, "HTTP Version Not Supported"));

	// *****************MIME_TYPES****************** //

	mimeTypes.insert(std::make_pair(".html", "text/html"));
	mimeTypes.insert(std::make_pair(".htm", "text/html"));
	mimeTypes.insert(std::make_pair(".css", "text/css"));
	mimeTypes.insert(std::make_pair(".csv", "text/csv"));
	mimeTypes.insert(std::make_pair(".doc", "application/doc"));
	mimeTypes.insert(std::make_pair(".gif", "image/gif"));
	mimeTypes.insert(std::make_pair(".jpg", "image/jpeg"));
	mimeTypes.insert(std::make_pair(".jpeg", "image/jpeg"));
	mimeTypes.insert(std::make_pair(".js", "text/javascript"));
	mimeTypes.insert(std::make_pair(".json", "application/json"));
	mimeTypes.insert(std::make_pair(".jar", "application/java-archive"));
	mimeTypes.insert(std::make_pair(".mp3", "audio/mpeg"));
	mimeTypes.insert(std::make_pair(".mp4", "video/mp4"));
	mimeTypes.insert(std::make_pair(".mpeg", "video/mpeg"));
	mimeTypes.insert(std::make_pair(".png", "image/png"));
	mimeTypes.insert(std::make_pair(".pdf", "application/pdf"));
	mimeTypes.insert(std::make_pair(".sh", "application/x-sh"));
	mimeTypes.insert(std::make_pair(".wav", "audio/wav"));
	mimeTypes.insert(std::make_pair(".weba", "audio/webm"));
	mimeTypes.insert(std::make_pair(".webm", "video/webm"));
	mimeTypes.insert(std::make_pair(".webp", "image/webp"));
	mimeTypes.insert(std::make_pair(".xml", "application/xml"));
	mimeTypes.insert(std::make_pair(".zip", "application/zip"));
	mimeTypes.insert(std::make_pair(".tar", "application/x-tar"));
	mimeTypes.insert(std::make_pair(".bin", "application/octet-stream"));
	mimeTypes.insert(std::make_pair(".avif", "image/avif"));
	mimeTypes.insert(std::make_pair(".avi", "video/x-msvideo"));
}

Response::Response(const Response& rhs)
{
	*this = rhs;
}

Response&	Response::operator=(const Response& rhs)
{
	if (this != &rhs)
    {
		// statusCode = rhs.statusCode;
		// requestedResource = rhs.requestedResource;
		// _Request = rhs._Request;
		// _Config = rhs._Config;
		// locationBlock = rhs.locationBlock;
		headers = rhs.headers;
		body = rhs.body;
        components = rhs.components;
		headersOffset = rhs.headersOffset;
        contentLength = rhs.contentLength;
		headersSent = rhs.headersSent;
		bodySent = rhs.bodySent;
		keepAlive = rhs.keepAlive;
	}
	return (*this);
}

void	Response::setComponents(std::string& method, std::string& uri, int& status,t_header& headers)
{
	this->components.method = method;
	this->components.target = uri;
	this->components.status = status;
	this->components.headers = headers;
}

bool	Response::formPath( void )
{
	std::vector<std::string>::iterator it;
	struct stat targetStat;

	components.target = components.root + components.target;
	if (stat(components.target.c_str(), &targetStat) == -1)
	{
		components.status = 404;
		return (false);
	}

	if (S_ISDIR(targetStat.st_mode))
	{
		if (access(components.target.c_str(), X_OK) != 0) // check exec permission to traverse dir
		{
			components.status = 403;
			return (false);
		}
		if (components.target.at(components.target.length() - 1) != '/')
			components.target.append("/");
		for (it = components.index.begin(); it != components.index.end(); it++)
		{
			if (access((components.target + *it).c_str(), F_OK) == 0) // file exists
				break;
		}
		if (it == components.index.end())
		{
			components.status = 404;
			return (false);
		}
		components.target.append(*it);
	}
	if (access(components.target.c_str(), R_OK) != 0) // read permission for files
	{
		components.status = 403;
		return (false);
	}
	return (true);
}

void	Response::errorResponse( void )
{
	std::cout << components.status << " " << errorCodes[components.status] << std::endl;
}

void	Response::generateErrorPage( void )
{
	 // if (error_page directive exists)
	 	// open the error page in bodyFD
	 //else
		body = "<html>\n<head><title> " + std::to_string(components.status) + " " + errorCodes[components.status] + " </title></head>\n<body>\n<center><h1> " + std::to_string(components.status) + " " + errorCodes[components.status] + " </h1></center>\n<hr><center>webserv/1.0</center>\n</body>\n</html>\n";
		bodySent = true;
}

void	Response::generateResponse( void )
{
	body.clear();
	headers.clear();
	if (components.root[components.root.length() - 1] == '/') // need to be taken to request parsing
		components.root.erase(components.root.length() - 1);
	// validateURI(); // also this needs to be taken to request parsing
	
	headers.append("HTTP/1.1 " + std::to_string(components.status) + " " + errorCodes[components.status]);
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());
	if (components.status >= 400)
	{
		generateErrorPage();
		headers.append("\r\nContent-Type: text/html");
		headers.append("\r\nContent-Length: " + std::to_string(body.size()));
	}
	else if (components.method == "GET") // check allowed methods
	{
		if(!formPath()) // be careful of ".."
		{
			generateResponse();
			return;
		}
		bodyFile.open(components.target);
		headers.append("\r\nContent-Type: " + contentType(components.target, mimeTypes));
		headers.append("\r\nContent-Length: " + std::to_string(fileLength(components.target)));
	}
	headers.append("\r\nConnection: " + std::string(keepAlive ? "Keep-Alive" : "Close"));
	headers.append("\r\n\r\n");
	headers.append(body);

	// setMatchingLocationBlock();
	// setRequestedResource();
	// components.status = _Request->getStatusCode();

    // if (this->components.method == "GET")
		// handleGET();
	// else if (method == "POST")
	// 	handlePOST();
	// else if (method == "DELETE")
	// 	handleDELETE();
}

int	Response::sendResponse( int& socket )
{
	if (!headersSent)
	{
		int bytesSent = send(socket, headers.c_str() + headersOffset, headers.length() - headersOffset, 0);
		if (bytesSent == -1)
		{
			std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
			return (-1);
		}
		else if (bytesSent < static_cast<int>(headers.length()))
		{
			headersOffset += bytesSent;
			return (0);
		}
		else
			headersSent = true;
	}


	if (!bodySent)
	{
		char buffer[SEND_BUFFER_SIZE] = {0};
		int bytesRead = bodyFile.read(buffer, SEND_BUFFER_SIZE).gcount();
		if (bytesRead > 0)
		{
			int bytesSent = send(socket, buffer, bytesRead, 0);
			if (bytesSent == -1)
			{
				std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
				return (-1);
			}
			else if (bytesSent < bytesRead)
			{
				bodyFile.seekg(bytesSent - bytesRead, std::ios::cur);
			}
		}
		else if (bytesRead == 0)
		{
			bodySent = true;
			bodyFile.close();
			*this = Response();
			return (1);
		}
		else
		{
			std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl;
			return (-1);
		}
	}
	bodySent = false;
	headersSent = false;
	return (1);
}
// void	Response::setRequest(Request* _Request)
// {
// 	this->_Request = _Request;
// }
