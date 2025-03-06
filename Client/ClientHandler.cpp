# include "ClientHandler.hpp"
# include "Response/Error.hpp"
# include "CGI/CGIHandler.hpp"
# include "../HTTPServer/Webserv.hpp"

ClientHandler::~ClientHandler()
{	
	HTTPserver->removeHandler(socket);
	HTTPserver->eraseTimer(socket);
	deleteResponse();
}

ClientHandler::ClientHandler(int fd, std::vector<ServerConfig>& vServers) : request(vServers), vServers(vServers)
{
	socket = fd;
	cgiActive = false;
	response = NULL;
	reqState = REGULAR;
}

void	ClientHandler::reset()
{
	reqState = REGULAR;
	deleteResponse();
	request = Request(vServers);
}

bool	ClientHandler::getCgiActive() const
{
	return (cgiActive);
}

int	ClientHandler::getFd() const
{
	return (socket);
}

void	ClientHandler::deleteResponse()
{
	if (response)
	{
		if (cgiActive)
		{
			CGIHandler *cgi = static_cast<CGIHandler *>(response);
			HTTPserver->eraseDependency(cgi);
			HTTPserver->collect(cgi);
			HTTPserver->decCgiCounter();
			cgiActive = false;
		}
		delete response;
		response = NULL;
	}
}

void	ClientHandler::createResponse()
{
	if (request.getRequestData()->isCGI)
	{
		if (HTTPserver->getCgiCounter() >= CGI_LIMIT)
			throw(Code(503));
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		cgiActive = true;
		HTTPserver->registerDependency(cgi, this);
		HTTPserver->registerHandler(cgi->getFd(), cgi, EPOLLIN);
		HTTPserver->updateHandler(socket, 0);
		response = cgi;
		HTTPserver->incCgiCounter();
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
		throw(Disconnect("\tClient " + _toString(socket) + " : Closed Connection"));
	else if (bytesReceived < 0)
		throw(Disconnect("\tClient " + _toString(socket) + " : recv: " + strerror(errno)));
	else
	{
		int returnValue;
		switch (reqState)
		{
			case REGULAR:
				returnValue = request.parseControlCenter(buf, bytesReceived);
				if (returnValue == FORWARD_CGI) // receive CGI body
				{
					if (HTTPserver->getCgiCounter() >= CGI_LIMIT)
						throw(Code(503));
					CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
					cgiActive = true;
					HTTPserver->registerDependency(cgi, this);
					HTTPserver->registerHandler(cgi->getFd(), cgi, 0);
					cgi->setBuffer(request.getBuffer());
					response = cgi;
					reqState = CGI;
					HTTPserver->incCgiCounter();
				}
				else if (returnValue == RESPOND) // receiving done - move to response
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
		std::cout << MAGENTA
					<< "\t" << request.getRequestData()->Method
					<< " " << request.getRequestData()->URI
					<< " " << request.getRequestData()->StatusCode
					<< RESET << std::endl;
		if (request.getRequestData()->keepAlive)
		{
			HTTPserver->updateTimer(socket);
			HTTPserver->updateHandler(socket, EPOLLIN);
			this->reset();
		}
		else
			throw(Disconnect());
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
			throw(Disconnect("\tClient " + _toString(socket) + " : Closed connection"));
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
		throw(Disconnect("\tClient " + _toString(socket) + " : Error on child process"));
	}
}
