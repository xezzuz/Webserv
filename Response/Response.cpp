#include "Response.hpp"

Response::~Response() {}

Response::Response() : contentLength(0), headersOffset(0), isDir(false), currRange(0),rangeOffset(0), state(SENDINGHEADER)
{
    statusCodes.insert(std::make_pair(200, "OK"));
    statusCodes.insert(std::make_pair(201, "Created"));
    statusCodes.insert(std::make_pair(204, "No Content"));
    statusCodes.insert(std::make_pair(206, "Partial Content"));
    statusCodes.insert(std::make_pair(301, "Moved Permanently"));
    statusCodes.insert(std::make_pair(302, "Found"));
    statusCodes.insert(std::make_pair(303, "See Other"));
    statusCodes.insert(std::make_pair(307, "Temporary Redirect"));
    statusCodes.insert(std::make_pair(308, "Permanent Redirect"));
    statusCodes.insert(std::make_pair(400, "Bad Request"));
    statusCodes.insert(std::make_pair(403, "Forbidden"));
    statusCodes.insert(std::make_pair(404, "Not Found"));
    statusCodes.insert(std::make_pair(405, "Method Not Allowed"));
    statusCodes.insert(std::make_pair(411, "Length Required"));
    statusCodes.insert(std::make_pair(413, "Payload Too Large"));
    statusCodes.insert(std::make_pair(415, "Unsupported Media Type"));
    statusCodes.insert(std::make_pair(500, "Internal Server Error"));
    statusCodes.insert(std::make_pair(501, "Not Implemented"));
    statusCodes.insert(std::make_pair(504, "Gateway Timeout"));
    statusCodes.insert(std::make_pair(505, "HTTP Version Not Supported"));

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
        input = rhs.input;
		contentType = rhs.contentType;
		contentLength = rhs.contentLength;
		headers = rhs.headers;
		body = rhs.body;
		headersOffset = rhs.headersOffset;
		isDir = rhs.isDir;
		ranges = rhs.ranges;
		boundary = rhs.boundary;
		endMark = rhs.endMark;
		currRange = rhs.currRange;
		rangeOffset = rhs.rangeOffset;
		state = rhs.state;
	}
	return (*this);
}

void	Response::setInput(struct ResponseInput& input)
{
	this->input = input;
}

bool	Response::removeResource( void )
{
	if (isDir)
	{
		if (rmdir(input.uri.c_str()) == -1)
		{
			input.status = 500;
			return (false);
		}
	}
	else 
	{
		if (remove(input.uri.c_str()) == -1)
		{
			input.status = 500;
			return (false);
		}
	}
	return (true);
}

void	Response::generateErrorPage( void )
{
	 // if (error_page directive exists)
	 	// open the error page in bodyFD
	 //else
		body = "<!DOCTYPE html>"
				"<html>"
				"<head>"
				"    <title> " + _toString(input.status) + " " + statusCodes[input.status] + " </title>"
				"</head>"
				"<body>"
				"    <center>"
				"        <h1> " + _toString(input.status) + " " + statusCodes[input.status] + " </h1>"
				"    </center>"
				"    <hr>"
				"    <center>webserv/1.0</center>"
				"</body>"
				"</html>";
}

void	Response::generatePostPage( void )
{
	body = "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"    <meta charset=\"UTF-8\">\n"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"    <title>Success Response</title>\n"
			"    <style>\n"
			"        body {\n"
			"            font-family: Arial, sans-serif;\n"
			"            margin: 40px auto;\n"
			"            max-width: 650px;\n"
			"            padding: 20px;\n"
			"        }\n"
			"        h1 {\n"
			"            color: #2c662d;\n"
			"        }\n"
			"        p {\n"
			"            color: #333;\n"
			"            line-height: 1.4;\n"
			"        }\n"
			"    </style>\n"
			"</head>\n"
			"<body>\n"
			"    <h1>Success</h1>\n"
			"    <p>Your request has been processed successfully.</p>\n"
			"    <p>The data has been saved to our system.</p>\n"
			"</body>\n"
			"</html>";
}

bool	Response::validateUri( void )
{
	struct stat targetStat;

	if (!rootJail(input.uri))
	{
		input.status = 403;
		return (false);
	}

	input.uri = input.config.root + input.uri;
	if (stat(input.uri.c_str(), &targetStat) == -1)
	{
		input.status = 404;
		return (false);
	}

	if (S_ISDIR(targetStat.st_mode))
	{
		isDir = true;
		if (access(input.uri.c_str(), X_OK) != 0) // check exec permission to traverse dir
		{
			input.status = 403;
			return (false);
		}
		if (input.uri.at(input.uri.length() - 1) != '/')
			input.uri.append("/");
	}
	return (true);
}

bool	Response::getResource( void )
{
	if (isDir)
	{
		std::vector<std::string>::iterator	it;

		for (it = input.config.index.begin(); it != input.config.index.end(); it++)
		{
			if (access((input.uri + *it).c_str(), F_OK) == 0) // file exists
				break;
		}
		if (it == input.config.index.end())
		{
			input.status = 404;
			return (false);
		}
		input.uri.append(*it);
	}
	if (access(input.uri.c_str(), R_OK) != 0) // read permission for files
	{
		input.status = 403;
		return (false);
	}
	bodyFile.open(input.uri);
	if (!bodyFile.is_open())
	{
		input.status = 500;
		return (false);
	}
	return (true);
}

bool	Response::parseRangeHeader( void ) // example => Range: bytes=0-499,1000-1499
{
	std::string	value = input.requestHeaders["range"];
	std::string prefix = "bytes=";
	size_t pos = value.find(prefix);
	if (pos == std::string::npos)
		return (false);
	pos += prefix.length();


	std::istringstream	rangess(value.substr(pos));
	std::string			rangeStr;

	boundary = generateRandomString();
	endMark = "\r\n--" + boundary + "--\r\n";

	while (std::getline(rangess, rangeStr, ','))
	{
		size_t delim;

		delim = rangeStr.find("-");
		if (delim == std::string::npos)
			return (false);
	
		std::string startStr = rangeStr.substr(0, delim);
		if (!allDigit(startStr))
			return (false);

		int start = stoi(startStr);
	

		std::string endStr = rangeStr.substr(delim + 1);
		if (!allDigit(startStr))
			return (false);

		int end = stoi(startStr);

		if (start > end || end >= contentLength)
			return (false);
	
		Range unit;

		unit.range = std::make_pair(start, end);

		unit.header.append("\r\n--" + boundary);
		unit.header.append("\r\nContent-Range: bytes " + startStr + "-" + endStr + "/" + _toString(contentLength));
		unit.header.append("\r\nContent-Type: " + contentType);
		unit.header.append("\r\n\r\n");

		ranges.push_back(unit);
	}
}

int	Response::rangeContentLength( void )
{
	int ret = 0;
	std::vector<Range>::iterator it = ranges.begin();

	ranges[0].header.erase(0, 2); // erase the first "\r\n"

	for (; it != ranges.end(); it++)
	{
		ret += it->header.length();
		ret += it->range.second - it->range.first + 1;
	}
	ret += endMark.length();
	return (ret);
}

bool	Response::buildRange( void )
{
	if (!parseRangeHeader())
		return (false);
	if (ranges.size() == 1)
	{
		ranges[0].header.clear();
		ranges[0].headerSent = true;
		headers.append("\r\nContent-Range: bytes " + _toString(ranges[0].range.first) + "-" + _toString(ranges[0].range.second) + "/" + _toString(fileLength(input.uri)));
		headers.append("\r\nContent-Length: " + _toString(ranges[0].range.second - ranges[0].range.first + 1));
		headers.append("\r\nContent-Type: " + contentType);
	}
	else
	{
		headers.append("\r\nContent-Type: multipart/byteranges; boundary=" + boundary);
		headers.append("\r\nContent-Length: " + rangeContentLength());
	}
}

void	Response::generateResponse( void )
{
	body.clear();
	headers.clear();
	
	if (input.status < 400)
		validateUri();

	// if (input.config.redirected)
	// {
	// 	input.status = input.config.redirect.first;
	// }
	headers.append("HTTP/1.1 " + _toString(input.status) + " " + statusCodes[input.status]);
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (input.status >= 400)
	{
		generateErrorPage();
		headers.append("\r\nContent-Type: text/html");
		headers.append("\r\nContent-Length: " + _toString(body.size()));
		input.requestHeaders["keep-alive"] = "close";
	}
	if (input.method == "GET") // check allowed methods
	{
		if (!getResource())
		{
			generateResponse();
			return ;
		}

		contentType = getContentType(input.uri, mimeTypes);
		contentLength = fileLength(input.uri);

		if (!(input.requestHeaders.find("range") != input.requestHeaders.end() && buildRange()))
		{
			headers.append("\r\nContent-Length: " + _toString(contentLength));
			headers.append("\r\nContent-Type: " + contentType);
		}
	}
	else if (input.method == "POST")
	{
		generatePostPage();
		headers.append("\r\nContent-Type: text/html");
		headers.append("\r\nContent-Length: " + _toString(body.length()));
	}
	else if (input.method == "DELETE")
	{
		if (!removeResource())
		{
			generateResponse();
			return ;
		}
	}
	if (input.requestHeaders.find("keep-alive") != input.requestHeaders.end())
		headers.append("\r\nConnection: " + input.requestHeaders["keep-alive"]);
	headers.append("\r\n\r\n");
	headers.append(body);
}

void	Response::sendRanges( int& socket )
{

	if (!ranges[currRange].headerSent)
	{
		int bytesSent = send(socket, ranges[currRange].header.c_str() + headersOffset, ranges[currRange].header.length() - headersOffset, 0);
		if (bytesSent == -1)
		{
			std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
			state = ERROR;
			return ;
		}
		else if (bytesSent < static_cast<int>(ranges[currRange].header.length()))
		{
			headersOffset += bytesSent;
			return ;
		}
		headersOffset = 0;
		ranges[currRange].headerSent = true;
	}


	bodyFile.seekg(ranges[currRange].range.first + rangeOffset, std::ios::beg);

	char buffer[SEND_BUFFER_SIZE] = {0};
	int readLength = std::min(SEND_BUFFER_SIZE, ranges[currRange].range.second - ranges[currRange].range.first - rangeOffset);
	int bytesRead = bodyFile.read(buffer, readLength).gcount();
	if (bytesRead > 0)
	{
		int bytesSent = send(socket, buffer, bytesRead, 0);
		if (bytesSent == -1)
		{
			std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
			state = ERROR;
			return ;
		}
		else if (bytesSent < bytesRead)
		{
			rangeOffset = bytesSent;
			return ;
		}
	}
	else if (bytesRead == -1)
	{
		std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl; // errno after I/O forbidden
		state = ERROR;
		return ;
	}

	if (readLength < ranges[currRange].range.second - ranges[currRange].range.first - rangeOffset) // this to determine if we sent the required range
	{
		if(++currRange >= ranges.size())
		{
			if (ranges.size() != 1)
				state = SENDINGENDMARK;
			else
				state = FINISHED;
			return ;
		}
	}
	rangeOffset = 0;
}

void	Response::sendBody( int& socket )
{
	char buffer[SEND_BUFFER_SIZE] = {0};
	int bytesRead = bodyFile.read(buffer, SEND_BUFFER_SIZE).gcount();
	if (bytesRead > 0)
	{
		int bytesSent = send(socket, buffer, bytesRead, 0);
		if (bytesSent == -1)
		{
			std::cerr << "[WEBSERV]\tsend: " << strerror(errno) << std::endl;
			state = ERROR;
			return ;
		}
		else if (bytesSent < bytesRead)
		{
			bodyFile.seekg(bytesSent - bytesRead, std::ios::cur);
		}
	}
	else if (bytesRead == 0)
	{
		state = FINISHED;
		return ;
	}
	else
	{
		std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl;
		state = ERROR;
		return ;
	}
}

void	Response::sendHeader(int& socket)
{
	int bytesSent = send(socket, headers.c_str() + headersOffset, headers.length() - headersOffset, 0);
	if (bytesSent == -1)
	{
		std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
		state = ERROR;
		return ;
	}
	else if (bytesSent < static_cast<int>(headers.length()))
	{
		headersOffset += bytesSent;
		return ;
	}
	headersOffset = 0;
	state = SENDINGBODY;
}

void	Response::sendEndMark( int& socket )
{
	int bytesSent = send(socket, endMark.c_str(), endMark.length(), 0);
	if (bytesSent == -1)
	{
		std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
		state = ERROR;
		return ;
	}
	state = FINISHED;
}


int	Response::sendResponse( int& socket )
{

	switch (state)
	{
		case SENDINGHEADER:
			sendHeader(socket);
		case SENDINGBODY:
			if (!body.empty())
				state = FINISHED;
			sendBody(socket);
		case SENDINGRANGES:
			sendRanges(socket);
		case SENDINGENDMARK:
			sendEndMark(socket);
		case FINISHED:
			return (1);
		case ERROR:
			return (-1);
	}


	// if (!headersSent)
	// {
	// 	int bytesSent = send(socket, headers.c_str() + headersOffset, headers.length() - headersOffset, 0);
	// 	if (bytesSent == -1)
	// 	{
	// 		std::cerr << "[WEBSERV]\t send: " << strerror(errno) << std::endl;
	// 		return (-1);
	// 	}
	// 	else if (bytesSent < static_cast<int>(headers.length()))
	// 	{
	// 		headersOffset += bytesSent;
	// 		return (0);
	// 	}
	// 	else
	// 	{
	// 		headersSent = true;
	// 		headersOffset = 0;
	// 	}
	// }


	// if (!bodySent)
	// {
	// 	// if (!ranges.empty())
	// 	// {
	// 	// 	sendRanges(socket);
	// 	// 	if (ranges.size > 1)
	// 	// 		send(socket, endMakr.c_str(), endMark.length());
	// 	// }
	// 	// else
	// 	// 	sendBody(socket);
	// 	char buffer[SEND_BUFFER_SIZE] = {0};
	// 	int bytesRead = bodyFile.read(buffer, SEND_BUFFER_SIZE).gcount();
	// 	if (bytesRead > 0)
	// 	{
	// 		int bytesSent = send(socket, buffer, bytesRead, 0);
	// 		if (bytesSent == -1)
	// 		{
	// 			std::cerr << "[WEBSERV]\tsend: " << strerror(errno) << std::endl;
	// 			return (-1);
	// 		}
	// 		else if (bytesSent < bytesRead)
	// 		{
	// 			bodyFile.seekg(bytesSent - bytesRead, std::ios::cur);
	// 			return (0);
	// 		}
	// 	}
	// 	else if (bytesRead == 0)
	// 	{
	// 		bodySent = true;
	// 		bodyFile.close();
	// 		*this = Response();
	// 		return (1);
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl;
	// 		return (-1);
	// 	}
	// }
	// bodySent = false;
	// headersSent = false;
	// return (1);
}
