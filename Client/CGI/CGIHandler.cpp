#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"
#include "../Response/Error.hpp"

CGIHandler::~CGIHandler()
{
	close(outfd);
	close(infd);
}

CGIHandler::CGIHandler(int& clientSocket, RequestData *data) : Response(clientSocket, data), outfd(-1), infd(-1)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw(FatalError(strerror(errno)));
	outfd = pipe_fd[0];
	infd = pipe_fd[1];
	headers = "HTTP/1.1 200 OK\r\n";
	headers.append("Server: webserv/1.0\r\n");
	headers.append("Connection: keep-alive\r\n");
	headers.append("Content-Type: text/plain\r\n");
	headers.append("Content-Length: 11\r\n");
	headers.append("\r\n\r\n");
}

pid_t CGIHandler::getPid() const
{
	return (pid);
}

std::string	headerToEnv(const std::string& header)
{
	std::string envVar = const_cast<std::string&>(header);

	if (header != "content-type" && header != "content-length")
		envVar.insert(0, "HTTP_");
	
	envVar.replace(envVar.begin(), envVar.end(), '-', '_');
	for (size_t i = 0; i < envVar.size(); i++)
		toupper(envVar[i]);
	envVar.append("=");
	return (envVar);
}

void	CGIHandler::buildEnv()
{

	// envvars.push_back("SERVER_NAME=" + reqCtx->config.);
	envvars.push_back("REQUEST_METHOD=" + reqCtx->Method);
	envvars.push_back("SCRIPT_NAME=" + reqCtx->scriptName);
	envvars.push_back("PATH_INFO=" + reqCtx->pathInfo);
	envvars.push_back("QUERY_STRING=" + reqCtx->queryString);
	envvars.push_back("SERVER_SOFTWARE=webserv/1.0");
	envvars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envvars.push_back("SERVER_PROTOCOL=HTTP/1.1");

	// headers to ENV
	std::map<std::string, std::string>::iterator header;
	for (header = reqCtx->Headers.begin(); header != reqCtx->Headers.end(); header++)
	{
		envvars.push_back(headerToEnv(header->first) + header->second);
	}
}



int	CGIHandler::setup()
{
	std::cout << "FULL PATH CGI << " << reqCtx->fullPath << std::endl;
	pid = fork();
	if (pid == -1)
	{
		close(outfd);
		close(infd);
		throw(FatalError(strerror(errno)));

	}
	else if (pid == 0)
	{
		close(outfd);
		if (dup2(infd, 1) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("dup2");
			close(infd);
			exit(errno);
		}
		close(infd);

		std::string dir = reqCtx->fullPath.substr(0, reqCtx->fullPath.find(reqCtx->scriptName));
		if (chdir(dir.c_str()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("chdir");
			exit(errno);
		}

		char *arg[3];
		arg[0] = const_cast<char *>(reqCtx->cgiIntrepreter.c_str());
		arg[1] = const_cast<char *>(reqCtx->scriptName.c_str());
		arg[2] = NULL;

		buildEnv();
		std::vector<char *>	env;

		for (std::vector<std::string>::iterator it = envvars.begin(); it != envvars.end(); it++)
		{
			env.push_back(const_cast<char *>(it->c_str()));
		}
		env.push_back(NULL);

		if (execve(arg[0], arg, env.data()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("execve");
			exit(errno);
		}
	}
	close(infd);
	// char buf[1024];
	// read(outfd, buf, 1024);
	// std::cout << "--------" << buf << "-------" << std::endl;
	return (outfd);
}

// void	processHeaders()
// {
// 	std::stringstream header(buffer);
// 	std::string field;

// 	std::map<std::string, std::string>					singleValue;
// 	std::map<std::string, std::vector<std::string> >	multiValue;


// 	while (getline(header, field, '\r'))
// 	{
		
// 	}
// }

bool	CGIHandler::parseCGIHeaders()
{
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		throw(500);

	headers = buffer.substr(0, pos + 2);

	std::stringstream header(buffer);
	std::string field;

	std::map<std::string, std::string>					singleValue;
	std::map<std::string, std::vector<std::string> >	multiValue;


	while (getline(header, field, '\r'))
	{
		
	}
	
	// if \r\n\r\n set headers bool to !headers;
	return (true);
}


void		CGIHandler::readCgi()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(outfd, buf, SEND_BUFFER_SIZE);
	std::cout << "BYTES READ: " << bytesRead << std::endl;
	if (bytesRead == -1)
	{
		throw(FatalError(strerror(errno)));
	}
	if (bytesRead > 0)
	{
		buffer = buildChunk(buf, bytesRead);
		state = WRITE;
	}
	else
	{
		buffer = buildChunk(NULL, 0);
		nextState = DONE;
	}
}

int		CGIHandler::feedCgi(const char *buf)
{
	int		bytesWritten = write(infd, buf, SEND_BUFFER_SIZE);
	if (bytesWritten == -1)
	{
		throw(FatalError(strerror(errno)));
	}
	return (bytesWritten);
}

void	CGIHandler::handleEvent(uint32_t events)
{
	if (events & EPOLLIN)
	{
		std::cout << "HELLO" << std::endl;
		readCgi();
		CGIState = BRIDGE;
		switch (CGIState)
		{
			case PARSE:
				parseCGIHeaders();
				break;
			case BRIDGE:
				HTTPserver->updateHandler(socket, EPOLLOUT | EPOLLHUP);
				HTTPserver->updateHandler(outfd, 0);
				break;
		}
	}
	// else if (events& EPOLLOUT)
	// {
	// 	feedCgi();
	// }
}