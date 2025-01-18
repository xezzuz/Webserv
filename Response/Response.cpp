#include "Response.hpp"

Response::~Response() {}

Response::Response() : contentLength(0), isDir(false), chunked(false), currRange(0),rangeOffset(0), state(BUILDHEADER), nextState(READBODY)
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
    statusCodes.insert(std::make_pair(413, "Payload Too Large"));
    statusCodes.insert(std::make_pair(415, "Unsupported Media Type"));
    statusCodes.insert(std::make_pair(416, "Range Not Satisfiable"));
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
		dataOffset = rhs.dataOffset;
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

void	Response::handleDELETE( void )
{
	if (isDir)
	{
		if (rmdir(absolutePath.c_str()) == -1)
		{
			input.status = 500; // wrong
			generateErrorPage();
			return ;
		}
	}
	else 
	{
		if (remove(absolutePath.c_str()) == -1)
		{
			input.status = 500; // wrong
			generateErrorPage();
			return ;
		}
	}
	nextState = FINISHED;
}

void	Response::generateErrorPage( void )
{

	 // if (error_page directive exists)
	 	// open the error page in bodyFD
	 //else
		data = "<!DOCTYPE html>"
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
	headers.append("\r\nContent-Type: text/html");
	headers.append("\r\nContent-Length: " + _toString(data.size()));
	input.requestHeaders["keep-alive"] = "close";
	nextState = FINISHED;
}

void	Response::handlePOST( void )
{
	data = "<!DOCTYPE html>\n"
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
	headers.append("\r\nContent-Type: text/html");
	headers.append("\r\nContent-Length: " + _toString(data.size()));
	nextState = FINISHED;
}

bool	Response::validateUri( void )
{
	struct stat targetStat;

	if (!rootJail(input.uri))
	{
		input.status = 403;
		return (false);
	}

	absolutePath = input.config.root + input.uri;
	if (stat(absolutePath.c_str(), &targetStat) == -1)
	{
		input.status = 404;
		return (false);
	}

	if (S_ISDIR(targetStat.st_mode))
	{
		isDir = true; // requested resource is a directory
		if (access(absolutePath.c_str(), X_OK) != 0) // check exec permission to traverse dir
		{
			input.status = 403;
			return (false);
		}
		if (absolutePath.at(absolutePath.length() - 1) != '/')
			absolutePath.append("/");
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
			if (access((absolutePath + *it).c_str(), F_OK) == 0) // file exists
				break;
		}
		if (it == input.config.index.end())
		{
			if (input.config.autoindex)
				return (true);
			input.status = 403;
			return (false);
		}
		absolutePath.append(*it);
	}
	if (access(absolutePath.c_str(), R_OK) != 0) // read permission for files
	{
		input.status = 403;
		return (false);
	}
	bodyFile.open(absolutePath);
	if (!bodyFile.is_open())
	{
		input.status = 500;
		return (false);
	}
	contentType = getContentType(absolutePath, mimeTypes);
	contentLength = fileLength(absolutePath);
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

		if (start > end || end >= contentLength || start < 0 || end < 0)
		{
			input.status = 416;
			return (false);
		}
	
		Range unit;

		unit.range = std::make_pair(start, end);
		unit.rangeLength = end - start + 1;
	
		unit.header.append("\r\n--" + boundary);
		unit.header.append("\r\nContent-Range: bytes " + startStr + "-" + endStr + "/" + _toString(contentLength));
		unit.header.append("\r\nContent-Type: " + contentType);
		unit.header.append("\r\n\r\n");

		ranges.push_back(unit);
	}
	ranges[0].header.erase(0, 2); // erase the first "\r\n"
}

int	Response::rangeContentLength( void )
{
	int ret = 0;
	std::vector<Range>::iterator it = ranges.begin();

	for (; it != ranges.end(); it++)
	{
		ret += it->header.length();
		ret += it->rangeLength;
	}
	ret += 8 + boundary.length(); // 8 is  the length of the constant "\r\n--" "--\r\n"
	return (ret);
}

void	Response::buildRange( void )
{
	if (!parseRangeHeader())
		return ;
	if (ranges.size() == 1)
	{
		ranges[0].header.clear();
		ranges[0].headerSent = true;
		contentLength = ranges[0].rangeLength;
		headers.append("\r\nContent-Range: bytes " + _toString(ranges[0].range.first) + "-" + _toString(ranges[0].range.second) + "/" + _toString(fileLength(absolutePath)));
	}
	else
	{
		contentType = "multipart/byteranges; boundary=" + boundary;
		contentLength = rangeContentLength();
	}
	nextState = NEXTRANGE;
}

void	Response::handleGET( void )
{
	if (!getResource())
	{
		generateErrorPage();
		return ;
	}

	if (input.config.autoindex && isDir)
	{
		headers.append("\r\nTransfer-Encoding: chunked");
		dirList = opendir(absolutePath.c_str());
		nextState = LISTDIR;
	}
	else
	{
		if (input.requestHeaders.find("range") != input.requestHeaders.end())
			buildRange();
		headers.append("\r\nContent-Length: " + _toString(contentLength));
	}
	headers.append("\r\nContent-Type: " + contentType);
}

void	Response::generateResponse( void )
{
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (input.status < 400)
		validateUri();

	if (input.status >= 400)
		generateErrorPage();

	if (input.method == "GET") // check allowed methods
		handleGET();
	else if (input.method == "POST")
		handlePOST();
	else if (input.method == "DELETE")
		handleDELETE();

	if (input.requestHeaders.find("keep-alive") != input.requestHeaders.end())
		headers.append("\r\nConnection: " + input.requestHeaders["keep-alive"]);

	headers.append("\r\n\r\n");

	headers = ("HTTP/1.1 " + _toString(input.status) + " " + statusCodes[input.status]) + headers; // status line
	data.insert(0, headers);
	state = SENDDATA;
}

void	Response::directoryListing()
{
	struct dirent	*entry;
	std::string 	chunk;
	int 			i = 0;

	while (i < 100 && (entry = readdir(dirList)) != NULL)
	{
		chunk.append("<a href=\"");
		chunk.append(entry->d_name);
		if (entry->d_type == DT_DIR)
			chunk.append("/");
		chunk.append(">");
		chunk.append(entry->d_name);
		if (entry->d_type == DT_DIR)
			chunk.append("/");
		
		chunk.append("</a>");
	}

	if (entry == NULL)
	{
		chunk.append("</pre><hr></body></html>\r\n0\r\n"); // the size does not include the CRLF carful
		nextState = FINISHED;
	}
	data = toHex(chunk.size()) + chunk + "\r\n";
	
}

void	Response::getNextRange()
{
	if (currRange >= ranges.size())
	{
		state = FINISHED;
		return;
	}
	data = ranges[currRange].header;
	state = SENDDATA;
	bodyFile.seekg(ranges[currRange].range.first, std::ios::beg);
	nextState = READRANGE;
}

void	Response::readRange()
{
	char buffer[SEND_BUFFER_SIZE] = {0};

	size_t readLength = std::min(
		static_cast<size_t>(SEND_BUFFER_SIZE),
		ranges[currRange].rangeLength
	);

	int bytesRead = bodyFile.read(buffer, readLength).gcount();
	if (bytesRead == -1)
	{
		std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl; // errno after I/O forbidden
		state = ERROR;
	}
	else if (bytesRead > 0)
	{
		data = buffer;
		ranges[currRange].rangeLength -= bytesRead;
		state = SENDDATA;
		if (ranges[currRange].rangeLength == 0)
		{
			nextState = NEXTRANGE;
			currRange++;
		}
	}
}

int	Response::readBody()
{
	char buffer[SEND_BUFFER_SIZE] = {0};
	int bytesRead = bodyFile.read(buffer, SEND_BUFFER_SIZE).gcount();
	if (bytesRead > 0)
	{
		data = buffer;
	}
	else if (bytesRead == 0)
	{
		state = FINISHED;
	}
	else
	{
		std::cerr << "[WEBSERV]\tread: " << strerror(errno) << std::endl;
		state = ERROR;
	}
}

bool	Response::sendData(int& socket, std::string& data)
{
	ssize_t bytesSent = send(socket, data.c_str(), data.length(), 0);
	if (bytesSent == -1)
	{
		std::cerr << "[WEBSERV]\tsend: " << strerror(errno) << std::endl;
		state = ERROR;
	}
	data.erase(0, bytesSent);
	return (data.empty());
}

int	Response::sendResponse( int& socket )
{
	switch (state)
	{
		case BUILDHEADER:
			generateResponse();
		case READBODY:
			readBody();
		case LISTDIR:
			directoryListing();
		case NEXTRANGE:
			getNextRange();
		case READRANGE:
			readRange();
		case SENDDATA:
			if(sendData(socket, headers) == true)
				state = nextState;
		case ERROR:
			return (-1);
		case FINISHED:
			return (1);
		default:
			return (0);
	}
}
		// case SENDINGENDMARK:
		// 	if (sendData(socket, endMark) == 1);
		// 		state = FINISHED;
