#include "ClientHandler.hpp"
#include "CGI/CGIHandler.hpp"
#include "../HTTPServer/Webserv.hpp"

ClientHandler::~ClientHandler()
{
	deleteResponse();
}

ClientHandler::ClientHandler(int fd, std::vector<ServerConfig>& vServers) : socket(fd), request(vServers), response(NULL), vServers(vServers), cgifd(-1), keepAlive(false), bridgeState(HEADERS) {}

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
	if (cgifd != -1)
	{
		HTTPserver->removeHandler(cgifd);
		cgifd = -1;
	}
	if (response)
	{
		delete response;
		response = NULL;
	}
}

void	ClientHandler::createResponse()
{
	if(request.getRequestData()->isCGI)
	{
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		cgi->setup();
		this->response = cgi;

		// if no body
		// cgifd = cgi->getOutfd();
		cgifd = cgi->getFd();
		std::cout << "CGIFD: " <<cgifd << std::endl;
		HTTPserver->registerHandler(cgifd, cgi, EPOLLIN | EPOLLHUP);
		HTTPserver->updateHandler(socket, EPOLLHUP);

		// if body()
		// cgifd = cgi->getInfd();
		// HTTPserver->registerHandler(cgifd, cgi, 0);
		// HTTPserver->updateHandler(socket, EPOLLIN | EPOLLHUP);
	}
	else
	{
		this->response = new Response(socket, request.getRequestData());
		HTTPserver->updateHandler(socket, EPOLLOUT | EPOLLHUP);
		response->generateHeaders();
	}
}

void 	ClientHandler::handleRequest()
{
	switch (bridgeState)
	{
		case HEADERS:
			if (request.feedRequest(socket) == REQUEST_FINISHED)
			{
				createResponse();
				bridgeState = BODY;
			}
			break;
		case BODY:
			{

			}
			break;
		case RESPOND:
			
			break;
	}
}

void 	ClientHandler::handleResponse()
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
			handleRequest();
		}
		else if (events & EPOLLOUT)
		{
			try
			{
				handleResponse();
			}
			catch (CGIRedirectException& redirect)
			{
				deleteResponse();
				decodeAbsPath(redirect.location, *request.getRequestData());
				createResponse();
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
