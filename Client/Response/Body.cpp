#include "Response.hpp"

// #include "Request.hpp"

// void	Request::storeBody( void )
// {
// 	uploader.open(std::tmpnam(NULL), std::ios::out | std::ios::trunc);
// 	if (!uploader.is_open())
// 		throw(500);

// 	uploader.write(_RequestRaws.rawBody.c_str(), _RequestRaws.rawBody.size());
// 	if (!uploader)
// 		throw(500);
// 	_RequestRaws.rawBody.clear(); //
// }

// // void			Request::putRequestBodyInFile() {
// // 	int fd = open("RequestBody.txt", O_CREAT | O_RDWR | O_TRUNC);
// // 	// std::tmpnam();
// // 	write(fd, _RequestRaws.rawBody.c_str(), _RequestRaws.bodySize);

// // 	std::cout << "******************** BODY ********************\n";
// // 	for (int i = 0; i <= (int)_RequestRaws.bodySize; i++)
// // 		std::cout << _RequestRaws.rawBody[i];
// // 	std::cout << "\n**********************************************\n";
// // }

// bool			Request::bufferContainChunk() {
// 	size_t			cpos = 0;
// 	size_t			CRLFpos = 0;
// 	std::string		chunkSizeStr;

// 	CRLFpos = buffer.find("\r\n");
// 	if (CRLFpos == std::string::npos)
// 		return false;
	
// 	chunkSizeStr = buffer.substr(cpos, CRLFpos - cpos);
// 	if (!isHexa(chunkSizeStr))
// 		return false;
	
// 	if (buffer.find("\r\n", CRLFpos + 2) == std::string::npos)
// 		return false;
// 	return true;
// }

// bool			Request::parseLengthBody() {
// 	if (!bufferSize)
// 		return true;
	
// 	_RequestRaws.rawBody += buffer.substr(0, bufferSize);
// 	_RequestRaws.bodySize += bufferSize;
// 	buffer.clear();
// 	bufferSize = 0;
// 	if (_RequestRaws.bodySize == _RequestData.contentLength)
// 		return (pState = BODY_FINISHED, true);
// 	if (_RequestRaws.bodySize > _RequestData.contentLength)
// 		return (setStatusCode(400), false);
// 	return true;
// }

// // unchunks available chunks and send them to raw Body
// bool			Request::decodeChunkedBody() {

// 	while (bufferContainChunk()) {
// 		std::string		chunkSizeStr;
// 		int			currPos = 0, CRLFpos = 0;
		
// 		CRLFpos = buffer.find("\r\n");
// 		// if (CRLFpos == std::string::npos || CRLFpos == currPos)
// 		// 	return false;
		
// 		chunkSizeStr = buffer.substr(currPos, CRLFpos - currPos);
// 		if (!isHexa(chunkSizeStr))
// 			return (setStatusCode(400), false);
		
// 		int chunkSize = htoi(chunkSizeStr);
// 		if (!chunkSize && buffer.substr(CRLFpos, 4) == "\r\n\r\n") {
// 			std::cout << "[INFO]\tBody Decoding is Done..." << std::endl;
// 			buffer.clear(), bufferSize = 0;
// 			pState = BODY_FINISHED;
// 			return true;
// 		} else if (!chunkSize && buffer.substr(CRLFpos, 4) != "\r\n\r\n")
// 			return (setStatusCode(400), false);

// 		currPos = CRLFpos + 2;
// 		if (bufferSize <= currPos + chunkSize + 2)
// 			return true;

// 		_RequestRaws.rawBody += buffer.substr(currPos, chunkSize);
// 		_RequestRaws.bodySize += chunkSize;
// 		// if (buffer.substr(currPos + chunkSize, 2) != "\r\n") // malformed chunk
// 		// 	return false;
// 		buffer.erase(0, currPos + chunkSize + 2);
// 		bufferSize -= currPos + chunkSize + 2;
// 	}
// 	return true;
// }

// bool			Request::processMultipartHeaders() {
// 	size_t			currPos, filenamePos, filenamePos_;
// 	std::string		contentDisposition;
// 	std::string		filename;

// 	currPos = _RequestRaws.boundaryBegin.size() + 2;
// 	contentDisposition = _RequestRaws.rawBody.substr(currPos, _RequestRaws.rawBody.find("\r\n", currPos));
// 	filenamePos = contentDisposition.find("filename=\"") + 10;
// 	if (filenamePos == std::string::npos)
// 		return (setStatusCode(400), false);
// 	filenamePos_ = contentDisposition.find("\"", filenamePos);
// 	if (filenamePos_ == std::string::npos)
// 		return (setStatusCode(400), false);
	
// 	filename = contentDisposition.substr(filenamePos, filenamePos_ - filenamePos);
// 	if (filename.empty())
// 		return (setStatusCode(400), false);
	
// 	std::cout << "creating " + filename << std::endl;
// 	std::string tmp = "/Users/nazouz/goinfre/Webserv/" + filename;
// 	int	fd = open(tmp.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
// 	if (fd == -1) {
// 		printf("hhhh\n");
// 		return (setStatusCode(500), false);
// 	}
	
// 	files.push_back(fd);
// 	currPos = _RequestRaws.rawBody.find("\r\n\r\n", currPos);
// 	if (currPos == std::string::npos)
// 		return (setStatusCode(400), false);
// 	currPos += 4;
// 	_RequestRaws.rawBody.erase(0, currPos);
// 	_RequestRaws.bodySize -= currPos;
// 	return true;
// }

// bool			Request::processMultipartData() {
// 	int				currentFile = files.back();
// 	size_t			bboundary = 0, eboundary = 0;

// 	bboundary = _RequestRaws.rawBody.find("\r\n" + _RequestRaws.boundaryBegin + "\r\n");
// 	eboundary = _RequestRaws.rawBody.find("\r\n" + _RequestRaws.boundaryEnd + "\r\n");
	
// 	std::cout << "writing to old file" << std::endl;
// 	int bytesToWrite = 0;
// 	if (bboundary == std::string::npos && eboundary == std::string::npos) {
// 		bytesToWrite = _RequestRaws.bodySize;
// 		int bytesWritten = write(currentFile, _RequestRaws.rawBody.c_str(), bytesToWrite);
// 		if (bytesWritten == -1) {
// 			printf("write");
// 			return (setStatusCode(500), false);
// 		}
// 		_RequestRaws.rawBody.erase(0, bytesWritten), _RequestRaws.bodySize -= bytesWritten;
// 		return true;
// 	} else if (bboundary != std::string::npos)
// 		bytesToWrite = bboundary;
// 	else
// 		bytesToWrite = eboundary;
	
// 	int bytesWritten = write(currentFile, _RequestRaws.rawBody.c_str(), bytesToWrite);
// 	if (bytesWritten == -1)
// 		return (setStatusCode(500), false);
	
// 	_RequestRaws.rawBody.erase(0, bytesWritten + 2);
// 	_RequestRaws.bodySize -= bytesWritten + 2;
// 	if (_RequestRaws.rawBody == _RequestRaws.boundaryEnd + "\r\n")
// 		_RequestRaws.rawBody.clear(), _RequestRaws.bodySize = 0;
// 	return true;
// }

// bool			Request::processMultipartFormData() {
// 	while (!_RequestRaws.rawBody.empty()) {
// 		// if rawBody contain boundaryBegin and Headers CRLF-CRLF
// 		if (_RequestRaws.rawBody.find(_RequestRaws.boundaryBegin + "\r\n") == 0 && _RequestRaws.rawBody.find("\r\n\r\n") != std::string::npos) {
// 			if (!processMultipartHeaders())
// 				return false;
// 		}
// 		if (_RequestRaws.rawBody.find(_RequestRaws.boundaryBegin + "\r\n") != 0 && files.size() > 0) {
// 			if (!processMultipartData())
// 				return false;
// 		}
// 	}
// 	return true;
// }

// bool			Request::processBinaryBody() {
// 	if (_RequestRaws.rawBody.empty())
// 		return true;
// 	// if (_RequestRaws.bodySize > _RequestData.contentLength)
// 	// 	return (setStatusCode(400), false);
// 	if (!files.size()) {
// 		std::time_t				time = std::time(NULL);
// 		std::stringstream 		ss;
// 		ss << "/Users/nazouz/goinfre/Webserv/" << "_clientfile" << time;
// 		std::string				filename(ss.str());
// 		int fd = open(filename.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
// 		if (fd == -1)
// 			return (setStatusCode(500), false);
// 		files.push_back(fd);
// 	}
// 	int bytesWritten = write(files.back(), _RequestRaws.rawBody.c_str(), _RequestRaws.bodySize);
// 	if (bytesWritten == -1)
// 		return (setStatusCode(500), false);
// 	_RequestRaws.rawBody.erase(0, bytesWritten);
// 	_RequestRaws.bodySize -= bytesWritten;
// 	return true;
// }

// bool			Request::processRequestRawBody() {
// 	// if (_RequestRaws.bodySize > _RequestData.contentLength)
// 	// 	return (setStatusCode(400), false);
// 	std::cout << "processRequestRawBody(1);" << std::endl;
// 	if (!_RequestRaws.bodySize)
// 		return true;
// 	std::cout << "processRequestRawBody(2);" << std::endl;
	
// 	if (isMultipart) {
// 		std::cout << "**************************** is multipart!" << std::endl;
// 		if (!processMultipartFormData())
// 			return false;
// 	} else if (!isMultipart) {
// 		std::cout << "**************************** is binary!" << std::endl;
// 		if (!processBinaryBody())
// 			return false;
// 	}
// 	std::cout << "processRequestRawBody(true);" << std::endl;
// 	return true;
// }

// // BODY CONTROL CENTER
// bool			Request::parseRequestBody() {		// store request body in raw Body
// 	std::cout << "[REQUEST]\tParsing Body..." << std::endl;
// 	if (isEncoded) {
// 		if (!decodeChunkedBody())
// 			return false;
// 	}
// 	else {
// 		if (!parseLengthBody())
// 			return false;
// 	}
// 	// processRequestRawBody();
// 	return processRequestRawBody();					// process raw Body
// }

// void	Response::POSTbody(char *buf, ssize_t size)
// {
// 	buffer += std::string(buf, size);
// 	// processs body
// 	storeBody();
// }

// void	Response::storeBody( void )
// {
// 	uploadFile.open(std::tmpnam(NULL), std::ios::out | std::ios::trunc);
// 	if (!uploadFile.is_open())
// 		throw(500);

// 	uploadFile.write(buffer.c_str(), buffer.size());
// 	if (!uploadFile)
// 		throw(500);
// 	buffer.clear(); //
// }
