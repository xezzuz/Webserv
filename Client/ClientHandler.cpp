#include "ClientHandler.hpp"
#include "CGI/CGIHandler.hpp"
#include "../HTTPServer/Webserv.hpp"

ClientHandler::~ClientHandler()
{
	HTTPserver->removeHandler(socket);
	deleteResponse();
}

ClientHandler::ClientHandler(int fd, std::vector<ServerConfig>& vServers) : socket(fd), request(vServers), response(NULL), vServers(vServers), cgiActive(false), keepAlive(false), bridgeState(HEADERS) {}

void	ClientHandler::reset()
{
	std::cout << "[WEBSERV]\tRESETING " << socket << ".." << std::endl;
	bridgeState = HEADERS;
	deleteResponse();
	request = Request(vServers);
}

void	ClientHandler::remove()
{
	std::cerr << "[WEBSERV]\tCLIENT " << socket << " REMOVED" << std::endl;
	deleteResponse();
	HTTPserver->removeHandler(socket);
	delete this;
}

int	ClientHandler::getSocket() const
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

	if(request.getRequestData()->isCGI)
	{
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		cgi->setup();
		cgiActive = true;
		this->response = cgi;
		HTTPserver->updateHandler(socket, EPOLLHUP);
	}
	else
	{
		this->response = new Response(socket, request.getRequestData());
	}
	if (!request.getBuffer().empty())
		this->response->setBuffer(request.getBuffer());
}

void 	ClientHandler::handleRead()
{
	char	buf[RECV_BUFFER_SIZE] = {0};

	ssize_t	bytesReceived = recv(socket, buf, RECV_BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		switch (bridgeState)
		{
			case HEADERS:
				if (request.parseControlCenter(buf, bytesReceived) == 1)
				{
					createResponse();
					bridgeState = BODY;
				}
				break;
			case BODY:
				response->handlePOST(buf, bytesReceived);
				break;
			case RESPOND:
				if (cgiActive)
					HTTPserver->updateHandler(socket, EPOLLHUP);
				else
					HTTPserver->updateHandler(socket, EPOLLOUT | EPOLLHUP);
				response->generateHeaders();
				break;
		}
	}
	else if (bytesReceived == 0)
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	else
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] recv: " + strerror(errno)));
}

void 	ClientHandler::handleWrite()
{
	if (response->respond() == 1)
	{
		std::cout << GREEN << "[WEBSERV][CLIENT-" + _toString(socket) + "]\tCLIENT SERVED" << RESET << std::endl;
		if (request.getRequestData()->keepAlive)
		{
			HTTPserver->updateHandler(socket, EPOLLIN | EPOLLHUP);
			this->reset();
		}
		else
			throw(Disconnect("[CLIENT-"+ _toString(socket) + "] CONNECTION CLOSE"));
	}
}

void	ClientHandler::handleEvent(uint32_t events)
{
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
			catch (CGIRedirectException& redirect)
			{
				deleteResponse();
				decodeAbsPath(redirect.location, *request.getRequestData()); ///////       ////
				createResponse(); ///// REWORK REWORK
			}
		}
	}
	catch (int& status)
	{
		deleteResponse();
		this->response = new Response(socket, request.getRequestData());
		HTTPserver->updateHandler(socket, EPOLLOUT | EPOLLHUP);
		response->generateErrorPage(status);
	}
	if (events & EPOLLHUP)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	}
}
