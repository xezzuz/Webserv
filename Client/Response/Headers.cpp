#include "Response.hpp"
#include "Error.hpp"

void	Response::handleGET( void )
{
	if (reqCtx->_Config->autoindex && reqCtx->isDir)
	{
		initDirList();
		headers.append("\r\nTransfer-Encoding: chunked");
		contentType = "text/html";
		reader = &Response::directoryListing;
	}
	else
	{
		bodyFile.open(reqCtx->fullPath.c_str()); // no protection
		if (!bodyFile.is_open())
			throw(500);
		contentType = getContentType(reqCtx->fullPath, mimeTypes);
		contentLength = fileLength(reqCtx->fullPath);
	
		if (reqCtx->Headers.find("range") != reqCtx->Headers.end())
			handleRange();
		headers.append("\r\nContent-Length: " + _toString(contentLength));
	}
	headers.append("\r\nContent-Type: " + contentType);
	headers.append("\r\nAccept-Ranges: bytes");
}

void	Response::generateHeaders( void )
{
	headers.append("\r\nServer: webserv/1.0");
	headers.append("\r\nDate: " + getDate());

	if (reqCtx->Method == "GET") // check allowed methods
		handleGET();
	// else if (reqCtx->Method == "POST")
	// 	handlePOST();
	else if (reqCtx->Method == "DELETE")
		nextState = DONE;

	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");

	headers.insert(0, "HTTP/1.1 " + _toString(reqCtx->StatusCode) + " " + statusCodes[reqCtx->StatusCode]); // status line
	headers.append("\r\n\r\n");
	sender = &Response::sendHeaders;
}