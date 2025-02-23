#include "Error.hpp"

ErrorPage::~ErrorPage() {}

ErrorPage::ErrorPage(int& status, int& socket, RequestData	*data) : AResponse(socket, data)
{
	this->status = status;
	statusCodes.insert(std::make_pair(400, "Bad Request"));
	statusCodes.insert(std::make_pair(403, "Forbidden"));
	statusCodes.insert(std::make_pair(404, "Not Found"));
	statusCodes.insert(std::make_pair(405, "Method Not Allowed"));
	statusCodes.insert(std::make_pair(413, "Payload Too Large"));
	statusCodes.insert(std::make_pair(414, "URI Too Long"));
	statusCodes.insert(std::make_pair(415, "Unsupported Media Type"));
	statusCodes.insert(std::make_pair(416, "Range Not Satisfiable"));
	statusCodes.insert(std::make_pair(431, "Request Header Fields Too Large"));
	statusCodes.insert(std::make_pair(500, "Internal Server Error"));
	statusCodes.insert(std::make_pair(501, "Not Implemented"));
	statusCodes.insert(std::make_pair(504, "Gateway Timeout"));
	statusCodes.insert(std::make_pair(505, "HTTP Version Not Supported"));
}

void	ErrorPage::readBody()
{
	char buf[SEND_BUFFER_SIZE] = {0};
	ssize_t bytesRead = bodyFile.read(buf, SEND_BUFFER_SIZE).gcount();
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] read: " + strerror(errno)));
	}
	else if (bytesRead > 0)
	{
		if (bodyFile.peek() == EOF)
		{
			nextState = DONE;
		}
		std::cout << YELLOW << "======[READ DATA OF SIZE " << bytesRead << "]======" << RESET << std::endl;
		buffer.append(std::string(buf, bytesRead));
		if ((this->*sender)() == true)
			state = nextState;
		else
			state = WRITE;
	}
}

void	ErrorPage::generateHeaders()
{
	headers.clear();
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());
	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");

	try
	{
		std::map<int, std::string>::iterator error_page = reqCtx->_Config->error_pages.find(reqCtx->StatusCode);
		if (error_page == reqCtx->_Config->error_pages.end())
			throw(status);

		fillRequestData(error_page->second, *reqCtx);
		bodyFile.open(reqCtx->fullPath.c_str());
		if (!bodyFile.is_open())
			throw(500);

		headers.append("\r\nContent-Type: " + getContentType(reqCtx->fullPath, mimeTypes));
		headers.append("\r\nContent-Length: " + _toString(fileLength(reqCtx->fullPath)));
		headers.append("\r\n\r\n");
	}
	catch (int& newStatus)
	{
		status = newStatus; // maybe remove
		buffer = "<!DOCTYPE html>\n"
				"<html>\n"
				"<head><title> " + _toString(newStatus) + " " + statusCodes[newStatus] + " </title></head>\n"
				"<body>\n"
				"<center><h1> " + _toString(newStatus) + " " + statusCodes[newStatus] + " </h1></center>\n"
				"<hr><center>webserv/1.0</center>\n"
				"</body>\n"
				"</html>";
		headers.append("\r\nContent-Type: text/html");
		headers.append("\r\nContent-Length: " + _toString(buffer.size()));
		headers.append("\r\n\r\n");
		headers.append(buffer);
		nextState = DONE;
	}
	headers.insert(0, "HTTP/1.1 " + _toString(status) + " " + statusCodes[status]);
	if ((this->*sender)() == true)
		state = nextState;
	else
		state = WRITE;
}

int	ErrorPage::respond()
{
	switch (state)
	{
		case HEADERS:
			generateHeaders();
			break;
		case READ:
			readBody();
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
