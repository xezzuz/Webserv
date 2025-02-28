#include "Response.hpp"

void	Response::handlePOST()
{
	buffer = "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"<meta charset=\"UTF-8\">\n"
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"<title>Success Response</title>\n"
			"<style>\n"
			"body {\n"
			"font-family: Arial, sans-serif;\n"
			"margin: 40px auto;\n"
			"max-width: 650px;\n"
			"padding: 20px;\n"
			"}\n"
			"h1 {\n"
			"color: #2c662d;\n"
			"}\n"
			"p {\n"
			"color: #333;\n"
			"line-height: 1.4;\n"
			"}\n"
			"</style>\n"
			"</head>\n"
			"<body>\n"
			"<h1>Success</h1>\n"
			"<p>Your request has been processed successfully.</p>\n"
			"<p>The data has been saved to our system.</p>\n"
			"</body>\n"
			"</html>";
	headers.append("\r\nContent-Type: text/html");
	headers.append("\r\nContent-Length: " + _toString(buffer.size()));
	headers.append(buffer);
	nextState = DONE;
}

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
		bodyFile.open(reqCtx->fullPath.c_str());
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

	if (reqCtx->Method == "GET")
		handleGET();
	else if (reqCtx->Method == "POST")
		handlePOST();
	else if (reqCtx->Method == "DELETE")
	{
		nextState = DONE;
		reqCtx->StatusCode = 204;
	}

	if (reqCtx->keepAlive)
		headers.append("\r\nConnection: keep-alive");
	else
		headers.append("\r\nConnection: close");

	headers.insert(0, "HTTP/1.1 " + _toString(reqCtx->StatusCode) + " " + statusCodes[reqCtx->StatusCode]); // status line
	headers.append("\r\n\r\n");
	if ((this->*sender)() == true)
		state = nextState;
	else
		state = WRITE;
}