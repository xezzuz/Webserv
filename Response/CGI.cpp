#include "Response.hpp"
#include "../Main/Webserv.hpp"

char	**Response::generateEnv()
{
	std::vector<std::string> envVars;
	// std::string	scriptName = input.uri.substr(input.uri.find(input.cgiExt) + input.cgiExt.length());
	
	envVars.push_back("REQUEST_METHOD=" + input.method);
	envVars.push_back("SCRIPT_NAME=" + input.cgi.scriptName);
	envVars.push_back("PATH_INFO=" + input.cgi.pathInfo); // input.uri should not be just the path info like that, think of a better way
	envVars.push_back("QUERYSTRING=" + input.cgi.queryString);
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	// needs headers to be formatted as env vars;
	std::vector<char *>	env;

	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); it++)
	{
		env.push_back(const_cast<char *>(it->c_str()));
	}
	env.push_back(NULL);
	return (env.data());
}


// execCGI needs :
// the path to the cgi interpreter
// the path to the script
bool	Response::execCGI( void )
{
	int fd[2];

	if (pipe(fd) == -1)
	{
		std::cerr << "[WEBSERV]\t";
		perror("pipe");
		return (false);
	}

	int pid = fork();
	if (pid == -1)
	{
		std::cerr << "[WEBSERV]\t";
		perror("fork");
		close(fd[0]);
		close(fd[1]);
		return (false);
	}
	else if (pid == 0)
	{
		close(fd[0]);
		if(dup2(fd[1], 1) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("dup2");
			exit(errno);
		}
		close(fd[1]);

		if (chdir(input.absolutePath.substr(0, input.absolutePath.find_last_of('/')).c_str()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("chdir");
			exit(errno);
		}

		std::vector<std::string> envVars;
		
		envVars.push_back("REQUEST_METHOD=" + input.method);
		envVars.push_back("SCRIPT_NAME=" + input.cgi.scriptName);
		envVars.push_back("PATH_INFO=" + input.cgi.pathInfo);
		envVars.push_back("QUERYSTRING=" + input.cgi.queryString);
		envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
		// needs headers to be formatted as env vars;
		std::vector<char *>	env;

		for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); it++)
		{
			env.push_back(const_cast<char *>(it->c_str()));
		}
		env.push_back(NULL);

		char *arg[3];
		arg[0] = const_cast<char *>(input.cgi.interpreter.c_str());
		arg[1] = const_cast<char *>(input.cgi.scriptName.c_str());
		arg[2] = NULL;
		if(execve(arg[0], arg, env.data()) == -1)
		{
			std::cerr << "[WEBSERV]\t";
			perror("execve");
			exit(errno);
		}
	}
	input.cgi.pid = pid;
	input.cgi.fd = fd[0];
	Webserv::addToPoll(input.cgi.fd, 0);
	nextState = READCGI;
	return (true);
}