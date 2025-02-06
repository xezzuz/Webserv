#include "Response.hpp"

Response::~Response()
{
	if (dirList)
		closedir(dirList);
}

Response::Response() : state(READ), nextState(READ)
{
	reader = &Response::readBody;
	sender = &Response::sendHeaders;
}

Response::Response(int &clientSocket) : socket(clientSocket), state(READ), nextState(READ)
{
	reader = &Response::readBody;
	sender = &Response::sendHeaders;
}

void	Response::setContext(struct RequestData	*ctx)
{
	reqCtx = ctx;
}

void	Response::setSocket(int& clientSocket)
{
	socket = clientSocket;
}

std::string	Response::buildChunk(const char *data, size_t size) // error
{
	return (toHex(size) + "\r\n" + std::string(data, size) + "\r\n");
}

void	Response::nextRange()
{
	if (reqCtx->rangeData.current == reqCtx->rangeData.ranges.end())
	{
		if (reqCtx->rangeData.ranges.size() > 1)
		{
			buffer = "\r\n--" + reqCtx->rangeData.boundary + "--\r\n";
			state = WRITE;
			nextState = DONE;
		}
		if (reqCtx->rangeData.ranges.size() == 1)//////// FIIIIX
		{
			state = DONE;
		}
	}
	else
	{
		// std::cout << "RANGE LENGTH OF INDEX " << reqCtx->rangeData.index << ": " << reqCtx->rangeData.ranges[reqCtx->rangeData.index].rangeLength << std::endl;
		if (reqCtx->rangeData.ranges.size() > 1)
			buffer.append(reqCtx->rangeData.current->header);
		bodyFile.seekg(reqCtx->rangeData.current->range.first, std::ios::beg);
		reqCtx->rangeData.rangeState = GET;
	}
}

void		Response::range()
{
	switch (reqCtx->rangeData.rangeState)
	{
		case NEXT:
			nextRange();
			break;
		case GET:
			readRange();
			break;
	}
}

bool	Response::sendHeaders()
{
	ssize_t bytesSent = send(socket, headers.c_str(), headers.length(), 0);
	if (bytesSent == -1)
	{
		throw(FatalError(strerror(errno)));
	}
	std::cout << headers;
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
		throw(FatalError(strerror(errno)));
	}
	std::cout << buffer;
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
		case DONE:
			return (1);
	}
	return (0);
}