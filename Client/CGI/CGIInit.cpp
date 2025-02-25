#include "CGIHandler.hpp"
#include "../../HTTPServer/Webserv.hpp"

std::string	headerToEnv(const std::string& headerKey, const std::string& headerValue)
{
	std::string envVar = "HTTP_";

	for (size_t i = 0; i < headerKey.size(); i++)
	{
		if (headerKey[i] == '-')
			envVar += '_';
		else
			envVar += std::toupper(headerKey[i]);
	}
	return (envVar + "=" + headerValue);
}

void	CGIHandler::buildEnv()
{
	// envvars.push_back("SERVER_NAME=" + reqCtx->config.);
	envVars.push_back("REQUEST_METHOD=" + reqCtx->Method);
	envVars.push_back("SCRIPT_NAME=" + reqCtx->scriptName);
	envVars.push_back("PATH_INFO=" + reqCtx->pathInfo);
	envVars.push_back("QUERY_STRING=" + reqCtx->queryString);
	envVars.push_back("SERVER_SOFTWARE=webserv/1.0");
	envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");

	// headers to ENV
	std::map<std::string, std::string>::iterator header;
	for (header = reqCtx->Headers.begin(); header != reqCtx->Headers.end(); header++)
	{
		if (header->first == "content-type")
			envVars.push_back("CONTENT_TYPE=" + header->second);
		else if (header->first == "content-length")
			envVars.push_back("CONTENT_LENGTH=" + header->second);
		else
			envVars.push_back(headerToEnv(header->first, header->second));
	}

	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); it++)
	{
		envPtr.push_back(const_cast<char *>(it->c_str()));
	}
	envPtr.push_back(NULL);
}

#include <cassert>
void	CGIHandler::execCGI()
{
	pid = fork();
	if (pid == -1)
	{
		close(pipe_in);
		close(pipe_out);
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] fork: " + strerror(errno)));

	}
	else if (pid == 0)
	{
		if (dup2(pipe_out, 1) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("dup2");
			close(pipe_in);
			close(pipe_out);
			exit(errno);
		}
		close(pipe_out);
	
		if (dup2(pipe_in, 0) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("dup2");
			close(pipe_in);
			exit(errno);
		}
		close(pipe_in);

		std::string dir = reqCtx->fullPath.substr(0, reqCtx->fullPath.find(reqCtx->scriptName));
		std::cout << dir << std::endl;
		if (chdir(dir.c_str()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("chdir");
			exit(errno);
		}

		args[0] = const_cast<char *>(reqCtx->cgiIntrepreter.c_str());
		args[1] = const_cast<char *>(reqCtx->scriptName.c_str());
		args[2] = NULL;

		buildEnv();
		if (execve(args[0], args, envPtr.data()) == -1)
		{
			std::cerr << "[WEBSERV][ERROR]\t";
			perror("execve");
			exit(errno);
		}
	}
	// close(pipe_out);
}
