#include "Error.hpp"
#include "Response.hpp"

FatalError::FatalError(const char *msg) : msg(msg) {}

const char *FatalError::what() const throw()
{
	return (msg);
}

void	Response::generateErrorPage(int& status)
{
	headers.clear();
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (reqCtx->_Config)
	{
		std::map<int, std::string>::iterator error_page = reqCtx->_Config->error_pages.find(status);
		if (error_page != reqCtx->_Config->error_pages.end())
		{
			status = 302;
			headers.append("\r\nLocation: " + error_page->second);
		}
	}
	buffer = "<!DOCTYPE html>\n"
			"<html>\n"
			"<head><title> " + _toString(status) + " " + statusCodes[status] + " </title></head>\n"
			"<body>\n"
			"<center><h1> " + _toString(status) + " " + statusCodes[status] + " </h1></center>\n"
			"<hr><center>webserv/1.0</center>\n"
			"</body>\n"
			"</html>";
	headers.append("\r\nContent-Type: text/html");
	headers.append("\r\nContent-Length: " + _toString(buffer.size()));
	if (reqCtx->keepAlive)
		headers.append("\r\nContent-Length: keep-alive");
	else
		headers.append("\r\nContent-Length: close");
	headers.append(buffer);
	headers.insert(0, "HTTP/1.1 " + _toString(status) + " " + statusCodes[status]) ; // status line
	nextState = DONE;
}