#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"

CGIHandler::~CGIHandler() {}

CGIHandler::CGIHandler(int& clientSocket) : Response(clientSocket), outfd(-1), infd(-1) {}

// int CGIHandler::getFd() const
// {
// 	return (fd);
// }

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

char	**CGIHandler::buildEnv()
{
	std::vector<std::string> envVars;

	// envVars.push_back("SERVER_NAME=" + reqCtx->config.);
	envVars.push_back("REQUEST_METHOD=" + reqCtx->Method);
	envVars.push_back("SCRIPT_NAME=" + reqCtx->scriptName);
	envVars.push_back("PATH_INFO=" + reqCtx->pathInfo);
	envVars.push_back("QUERYSTRING=" + reqCtx->queryString);
	envVars.push_back("SERVER_SOFTWARE=webserv/1.0");
	envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");

	// headers to ENV
	std::map<std::string, std::string>::iterator header;
	for (header = reqCtx->Headers.begin(); header != reqCtx->Headers.end(); header++)
	{
		envVars.push_back(headerToEnv(header->first) + header->second);
	}
	// needs headers to be formatted as env vars;

	std::vector<char *>	env;

	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); it++)
	{
		env.push_back(const_cast<char *>(it->c_str()));
	}
	env.push_back(NULL);
	return (env.data());
}

int	CGIHandler::setup()
{
	int pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "[WEBSERV]\t";
		perror("pipe");
		return (-1);
	}

	int pid = fork();
	if (pid == -1)
	{
		std::cerr << "[WEBSERV]\t";
		perror("fork");
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	else if (pid == 0)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], 1) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			close(pipe_fd[1]);
			perror("dup2");
			exit(errno);
		}
		close(pipe_fd[1]);

		if (chdir(reqCtx->fullPath.c_str()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("chdir");
			exit(errno);
		}

		char *arg[3];
		arg[0] = const_cast<char *>(reqCtx->cgiIntrepreter.c_str());
		arg[1] = const_cast<char *>(reqCtx->scriptName.c_str());
		arg[2] = NULL;
		if (execve(arg[0], arg, buildEnv()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("execve");
			exit(errno);
		}
	}
	pid = pid;
	outfd = pipe_fd[0];
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
		throw(ErrorPage(500));

	headers = buffer.substr(0, pos + 2);

	std::stringstream header(buffer);
	std::string field;

	std::map<std::string, std::string>					singleValue;
	std::map<std::string, std::vector<std::string> >	multiValue;


	while (getline(header, field, '\r'))
	{
		
	}
	
	// if \r\n\r\n set headers bool to !headers;
}


void		CGIHandler::readCgi()
{
	char	buf[SEND_BUFFER_SIZE] = {0};
	int		bytesRead = read(outfd, buf, SEND_BUFFER_SIZE);
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
		readCgi();
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