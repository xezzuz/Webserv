#include "../Response.hpp"
#include "../Error.hpp"

void	Response::buildChunk()
{
	buffer = toHex(buffer.size()) + "\r\n" + buffer + "\r\n";
	if (nextState == FINISHED)
		buffer.append("0\r\n\r\n");
	state = SENDDATA;
}

void	Response::readBody()
{
	char buf[SEND_BUFFER_SIZE] = {0};
	int bytesRead = bodyFile.read(buf, SEND_BUFFER_SIZE).gcount();
	std::cout << bytesRead << std::endl;
	if (bytesRead == -1)
	{
		throw (FatalError(strerror(errno)));
	}
	if (bytesRead > 0)
	{
		buffer.append(std::string(buf, bytesRead));
		state = SENDDATA;
		if (chunked)
		{
			buildChunk();
			if (bodyFile.peek() == EOF)
				nextState = FINISHED;
		}
	}
	else if (bytesRead == 0)
	{
		state = FINISHED;
	}
}

void	Response::handleGET( void )
{
	if (input.config.autoindex && input.isDir)
	{
		dirList = opendir(input.path.c_str()); // CHECK LEAK
		if (dirList == NULL)
		{
			std::cerr << "[WEBSERV][ERROR]\t>";
			perror("opendir");
			throw(ErrorPage(500));
		}
		headers.append("\r\nTransfer-Encoding: chunked");
		contentType = "text/html";
		state = AUTOINDEX;
	}
	else
	{
		contentType = getContentType(input.path, mimeTypes);
		contentLength = fileLength(input.path);
		if (input.requestHeaders.find("Range") != input.requestHeaders.end())
			buildRange();
		if (contentType.find("video") != std::string::npos || contentType.find("audio") != std::string::npos)
		{
			headers.append("\r\nTransfer-Encoding: chunked");
			chunked = true;
			state = SENDDATA;
			nextState = READBODY;
		}
		else
			headers.append("\r\nContent-Length: " + _toString(contentLength));
		openBodyFile(input.path);
	}
	headers.append("\r\nContent-Type: " + contentType);
	headers.append("\r\nAccept-Ranges: bytes");
}
