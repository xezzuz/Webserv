#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"
#include "../Response/Error.hpp"

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

void	CGIHandler::setup()
{
	pid = fork();
	if (pid == -1)
	{
		close(infd);
		close(outfd);
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] fork: " + strerror(errno)));

	}
	else if (pid == 0)
	{
		if (dup2(outfd, 1) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("dup2");
			close(infd);
			close(outfd);
			exit(errno);
		}
		close(outfd);
	
		if (dup2(infd, 0) == -1)
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
			std::cerr << "[WEBSERV][ERROR]\t";
			perror("execve");
			exit(errno);
		}
	}
	if (reqCtx->Method == "POST")
		HTTPserver->registerHandler(outfd, this, EPOLLIN | EPOLLHUP);
	else
		close(outfd);
}
