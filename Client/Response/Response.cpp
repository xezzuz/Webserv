#include "Response.hpp"
#include "Error.hpp"

Response::~Response()
{
	if (dirList)
		closedir(dirList);
}

Response::Response() : state(WRITE), nextState(READ)
{
	reader = &Response::readBody;
	sender = &Response::sendHeaders;
	dirList = NULL;
	reqCtx = NULL;
}

Response::Response(int &clientSocket, RequestData *data) : socket(clientSocket), state(WRITE), nextState(READ), reqCtx(data)
{
	dirList = NULL;
	reader = &Response::readBody;
	sender = &Response::sendHeaders;

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
    statusCodes.insert(std::make_pair(431, "Request Header Fields Too Large"));
    statusCodes.insert(std::make_pair(500, "Internal Server Error"));
    statusCodes.insert(std::make_pair(501, "Not Implemented"));
    statusCodes.insert(std::make_pair(504, "Gateway Timeout"));
    statusCodes.insert(std::make_pair(505, "HTTP Version Not Supported"));

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

Response& Response::operator=(const Response& rhs)
{
	if (this != &rhs)
	{
		socket = rhs.socket;
		state = rhs.state;
		nextState = rhs.nextState;
		headers = rhs.headers;
		buffer = rhs.buffer;
		sender = rhs.sender;
		reqCtx = rhs.reqCtx;
		rangeData = rhs.rangeData;
		dirList = rhs.dirList;
		contentType = rhs.contentType;
		contentLength = rhs.contentLength;
	}
	return (*this);
}

std::string	Response::buildChunk(const char *data, size_t size) // error
{
	return (toHex(size) + "\r\n" + std::string(data, size) + "\r\n");
}

bool	Response::sendHeaders()
{
	ssize_t bytesSent = send(socket, headers.c_str(), headers.length(), 0);
	if (bytesSent == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] send: " + strerror(errno)));
	}
	std::cout << GREEN << "======[SENT DATA OF SIZE " << bytesSent << " (HEADERS)]======" << RESET << std::endl;
	// std::cout << "__________HEADERS SENT_____________" << std::endl;
	// std::cout << headers;
	// std::cout << "___________________________________" << std::endl;
	headers.erase(0, bytesSent);
	if (headers.empty())
		sender = &Response::sendBody;
	return (headers.empty());
}

bool	Response::sendBody()
{
	ssize_t bytesSent = send(socket, buffer.c_str(), buffer.length(), 0);
	if (bytesSent == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] send: " + strerror(errno)));
	}
	std::cout << GREEN << "======[SENT DATA OF SIZE " << bytesSent << " (BODY)]======" << RESET << std::endl;
	// std::cout << "__________BODY SENT__" << bytesSent << "___________" << std::endl;
	// std::cout << buffer;
	// std::cout << "________________________________" << std::endl;
	buffer.erase(0, bytesSent);
	return (buffer.empty());
}

int	Response::respond()
{
	switch (state)
	{
		case READ:
			(this->*reader)();
			break;
		case WRITE:
			if ((this->*sender)() == true)
				state = nextState;
			break;
		case DONE:
			return (1);
	}
	return (0);
}