# include "ClientHandler.hpp"
# include "Response/Error.hpp"
# include "CGI/CGIHandler.hpp"
# include "../HTTPServer/Webserv.hpp"

ClientHandler::~ClientHandler()
{
	HTTPserver->removeHandler(socket);
	deleteResponse();
}

ClientHandler::ClientHandler(int fd, std::vector<ServerConfig>& vServers) : request(vServers), vServers(vServers)
{
	socket = fd;
	response = NULL;
	cgiActive = false;
	keepAlive = false;
	reqState = REGULAR;
	elapsedTime = std::time(NULL);
}

time_t	ClientHandler::getElapsedTime() const
{
	return (elapsedTime);
}

void	ClientHandler::ctime()
{
	elapsedTime = std::time(NULL);
}

void	ClientHandler::reset()
{
	std::cout << "[WEBSERV][CLIENT-" << socket << "]\tRESETING.." << std::endl;
	reqState = REGULAR;
	deleteResponse();
	request = Request(vServers);
}

int	ClientHandler::getFd() const
{
	return (socket);
}

void	ClientHandler::deleteResponse()
{
	if (response)
	{
		delete response;
		response = NULL;
	}
	cgiActive = false;
}

void	ClientHandler::createResponse()
{
	if (request.getRequestData()->isCGI)
	{
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		cgi->execCGI();
		HTTPserver->registerHandler(cgi->getFd(), cgi, EPOLLIN);
		HTTPserver->updateHandler(socket, 0);
		response = cgi;
		cgiActive = true;
	}
	else
	{
		this->response = new Response(socket, request.getRequestData());
		HTTPserver->updateHandler(socket, EPOLLOUT);
	}
}

void 	ClientHandler::handleRead()
{
	char	buf[RECV_BUFFER_SIZE] = {0};

	ssize_t	bytesReceived = recv(socket, buf, RECV_BUFFER_SIZE, 0);
	if (bytesReceived == 0)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	else if (bytesReceived < 0)
	{
		std::cerr << "[WEBSERV][ERROR]\t recv: " << strerror(errno) << std::endl;
		throw(500);
	}
	else
	{
		int retVal;
		switch (reqState)
		{
			case REGULAR:
				retVal = request.parseControlCenter(buf, bytesReceived);
				if (retVal == 1) // receive CGI body
				{
					CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
					cgi->execCGI();
					HTTPserver->registerHandler(cgi->getFd(), cgi, 0);
					cgi->setBuffer(request.getBuffer());
					response = cgi;
					cgiActive = true;
					reqState = CGI;
				}
				else if (retVal == 2) // receiving done - move to response
					createResponse();
				break;
			case CGI:
				static_cast<CGIHandler *>(response)->setBuffer(buf, bytesReceived);
				break;
		}
	}
}

void 	ClientHandler::handleWrite()
{
	if (response->respond() == 1)
	{
		std::cout << GREEN << "[WEBSERV][CLIENT-" << socket << "]\tCLIENT SERVED" << RESET << std::endl;
		if (request.getRequestData()->keepAlive)
		{
			this->elapsedTime = std::time(NULL);
			HTTPserver->updateHandler(socket, EPOLLIN);
			this->reset();
		}
		else
			throw(Disconnect("[CLIENT-" + _toString(socket) + "] CONNECTION CLOSE"));
	}
}

void	ClientHandler::handleEvent(uint32_t events)
{
	this->elapsedTime = std::time(NULL);
	try
	{
		if (events & EPOLLIN)
		{
			handleRead();
		}
		else if (events & EPOLLOUT)
		{
			handleWrite();
			// try
			// {
			// }
			// catch (CGIRedirectException& redirect)
			// {
			// 	deleteResponse();
			// 	decodeAbsPath(redirect.location, *request.getRequestData()); ///////       ////
			// 	createResponse(); ///// REWORK REWORK
			// }
		}
	}
	catch (int& status)
	{
		deleteResponse();
		this->response = new ErrorPage(status, socket, request.getRequestData());
		HTTPserver->updateHandler(socket, EPOLLOUT);
	}
	if (events & EPOLLHUP)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	}
}
