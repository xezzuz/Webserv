# include "ClientHandler.hpp"
# include "Response/Error.hpp"
# include "CGI/CGIHandler.hpp"
# include "../HTTPServer/Webserv.hpp"

ClientHandler::~ClientHandler()
{
	std::cout << "CLIENT CLEANUP" << std::endl;
	HTTPserver->removeHandler(socket);
	std::cout << "ERASING TIMER" << std::endl;
	HTTPserver->eraseTimer(socket);
	std::cout << "DELETING RESPONSE" << std::endl;
	deleteResponse();
}

ClientHandler::ClientHandler(int fd, std::vector<ServerConfig>& vServers) : request(vServers), vServers(vServers)
{
	socket = fd;
	response = NULL;
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
	std::cout << GREEN << "[WEBSERV][CLIENT-" << socket << "]\tRESETTING.." << RESET << std::endl;
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
		std::cout << "RESPONSE POINTER: " << response << std::endl;
		delete response;
		response = NULL;
	}
	else
		std::cout << "RESPONSE IS NULL" << std::endl;

}

void	ClientHandler::createResponse()
{
	if (request.getRequestData()->isCGI)
	{
		std::cout << "CGI NORMAL" << std::endl;
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		HTTPserver->registerDependency(cgi, this);
		HTTPserver->registerHandler(cgi->getFd(), cgi, EPOLLIN);
		HTTPserver->updateHandler(socket, 0);
		response = cgi;
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
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] recv: " + strerror(errno)));
	else
	{
		int returnValue;
		switch (reqState)
		{
			case REGULAR:
				returnValue = request.parseControlCenter(buf, bytesReceived);
				if (returnValue == RECV)
				std::cout << "RECV" << std::endl;
				if (returnValue == FORWARD_CGI) // receive CGI body
				{
					std::cout << "FORWARD_CGI" << std::endl;
					CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
					HTTPserver->registerDependency(cgi, this);
					HTTPserver->registerHandler(cgi->getFd(), cgi, 0);
					cgi->setBuffer(request.getBuffer());
					response = cgi;
					reqState = CGI;
				}
				else if (returnValue == RESPOND) // receiving done - move to response
				{
					std::cout << "RESPOND" << std::endl;
					createResponse();
				}
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
			HTTPserver->updateTimer(socket);
			HTTPserver->updateHandler(socket, EPOLLIN);
			this->reset();
		}
		else
			throw(Disconnect("[CLIENT-" + _toString(socket) + "] CONNECTION CLOSE"));
	}
}

void	ClientHandler::handleEvent(uint32_t events)
{
	HTTPserver->updateTimer(socket);
	try
	{
		if (events & EPOLLIN)
		{
			handleRead();
		}
		else if (events & EPOLLOUT)
		{
			try
			{
				handleWrite();
			}
			catch (CGIRedirect& redirect)
			{
				deleteResponse();
				request.setFullPath(redirect.location);
				resolveAbsPath(*request.getRequestData());
				createResponse();
			}
		}
		else if (events & EPOLLHUP)
		{
			throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
		}
	}
	catch (Code& e)
	{
		deleteResponse();
		this->response = new ErrorPage(e, socket, request.getRequestData());
		HTTPserver->updateHandler(socket, EPOLLOUT);
	}
	catch (ChildException& e)
	{
		HTTPserver->stop();
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	}
}
