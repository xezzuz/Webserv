#include "Response.hpp"
#include "Error.hpp"

int	Response::rangeContentLength( void )
{
	int length = 0;
	std::vector<Range>::iterator it = reqCtx->rangeData.ranges.begin();

	for (; it != reqCtx->rangeData.ranges.end(); it++)
	{
		length += it->header.length();
		length += it->rangeLength;
	}
	length += 8 + reqCtx->rangeData.boundary.length(); // 8 is  the length of the constant "\r\n--" "--\r\n" // FIX
	return (length);
}

void	Response::handleRange()
{
	if (reqCtx->rangeData.ranges.size() == 1)
	{
		contentLength = reqCtx->rangeData.current->rangeLength;
		headers.append("\r\nContent-Range: bytes "
			+ _toString(reqCtx->rangeData.current->range.first)
			+ "-"
			+ _toString(reqCtx->rangeData.current->range.second)
			+ "/"
			+ _toString(contentLength));
	}
	else
	{
		contentType = "multipart/byteranges; boundary=" + reqCtx->rangeData.boundary;
		contentLength = rangeContentLength();
	}
	reader = &Response::readRange;
}

void	Response::handleGET( void )
{
	if (reqCtx->config->autoindex && reqCtx->isDir)
	{
		dirList = opendir(reqCtx->path.c_str());
		if (!dirList)
		{
			throw(ErrorPage(500));
		}
		headers.append("\r\nTransfer-Encoding: chunked");
		contentType = "text/html";
		buffer = "<html>\n"
			"<head>\n"
			"<title>Index of " + reqCtx->uri + "</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1>Index of " + reqCtx->uri + "</h1>\n"
			"<hr>\n"
			"<pre>\n";
		reader = &Response::directoryListing;
	}
	else
	{
		bodyFile.open(reqCtx->path); // no protection
		contentType = getContentType(reqCtx->path, mimeTypes);
		contentLength = fileLength(reqCtx->path);
	
		if (reqCtx->isRange)
			handleRange();
		headers.append("\r\nContent-Length: " + _toString(contentLength));
	}
	headers.append("\r\nContent-Type: " + contentType);
	headers.append("\r\nAccept-Ranges: bytes");
}

void	Response::generateHeaders( void )
{
	headers = "HTTP/1.1 " + _toString(reqCtx->status) + " " + statusCodes[reqCtx->status]; // status line
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (reqCtx->method == "GET") // check allowed methods
		handleGET();
	else if (reqCtx->method == "POST")
		handlePOST();
	else if (reqCtx->method == "DELETE")
		nextState = DONE;

	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");

	headers.append("\r\n\r\n");
	sender = &Response::sendHeaders;
}