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
	deleteResponse();
	bridgeState = HEADERS;
	response = NULL;
	request = Request(vServers);
}

void	ClientHandler::remove()
{
	std::cerr << "[WEBSERV]\tCLIENT " << socket << " REMOVED" << std::endl;
	deleteResponse();
	HTTPserver->removeHandler(socket);
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
	else if (response)
		delete response;
}

void	ClientHandler::createResponse()
{
	if(request.getRequestData()->isCGI)
	{
		CGIHandler	*cgi = new CGIHandler(socket, request.getRequestData());
		cgifd = cgi->getFd();
		cgi->setup();
		std::cout << "CGI FD IS: " << cgifd << std::endl;
		HTTPserver->registerHandler(cgifd, cgi, EPOLLIN | EPOLLHUP);
		HTTPserver->updateHandler(socket, EPOLLHUP);
		this->response = cgi;
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
	if (bridgeState == HEADERS)
	{
		std::cout << BLUE << "ClientHandler::handleRequest Bridge : HEADERS" << RESET << std::endl;
		int		rState = request.feedRequest(socket);
		std::cout << BLUE << "ClientHandler::handleRequest rState : " << rState << RESET << std::endl;
		if (rState == REQUEST_FINISHED)
		{
			createResponse();
			bridgeState = BODY;
		}
		else if (rState == -1)
		{
			std::cerr << "ERROR>>>>>>>>>>>>>>>>>>>>>>>." << std::endl;
			this->remove();
		}
	}
	else if (bridgeState == BODY) // FORWARD RECV TO RESPONSE
	{
		// int		rState = response.feedResponse(socket);
		std::cout << BLUE << "ClientHandler::handleRequest Bridge : BODY" << RESET << std::endl;
		// char buf[16000];
		// int bytesRead = read(socket, buf, 16000);
		// if (bytesRead == -1)
		// {
		// 	std::cerr << " ERROROARORORO ::: " << strerror(errno) << std::endl;
		// }
		// std::cout << "BYTES_READ ON BODY => " << bytesRead << std::endl; 
		// buf[bytesRead] = '\0';
		// std::cout << "DATA_READ ON BODY => " << buf;
	}
}

void 	ClientHandler::handleResponse()
{
	if (response->respond() == 1)
	{
		std::cout << "[WEBSERV]\tCLIENT SERVED" << std::endl;
		if (request.getRequestData()->keepAlive)
		{
			HTTPserver->updateHandler(socket, EPOLLIN | EPOLLHUP);
			this->reset();
		}
		else
			this->remove(); // this call is very unsafe it should remain at the end of an EventHandler object Call T-T // eventually use vector
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
				HTTPserver->removeHandler(cgifd);
				
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
}
