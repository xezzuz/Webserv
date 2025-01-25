#include "Response.hpp"

char	**Response::generateEnv()
{
	std::vector<std::string> envVars;
	std::string	scriptName = input.uri.substr(input.uri.find(input.cgiExt) + input.cgiExt.length());
	
	envVars.push_back("REQUEST_METHOD=" + input.method);
	envVars.push_back("SCRIPT_NAME=" + scriptName);
	envVars.push_back("PATH_INFO=" + input.uri); // input.uri should not be just the path info like that, think of a better way
	envVars.push_back("QUERYSTRING=" + input.queryString);
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	// needs headers to be formatted as env vars;
	std::vector<char *>	env;

	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); it++)
	{
		env.push_back(&(*(it))[0]);
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
	if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
		char *arg[2];
		arg[0] = const_cast<char *>(input.absolutePath.c_str());
		arg[1] = NULL;
		execve(input.cgiInterpreter.c_str(), arg, generateEnv());
		// execScript();
	}
	else if (pid == -1)
	{
		std::cerr << "[WEBSERV]\t";
		perror("fork");
		close(fd[0]);
		close(fd[1]);
		return (false);
	}
	return (true); // i just added this statement due to compiler errors
}