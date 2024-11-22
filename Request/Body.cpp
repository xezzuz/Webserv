/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:30:24 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/22 17:10:58 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void			Request::putRequestBodyInFile() {
	int fd = open("RequestBody.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	
	write(fd, body.rawBody.c_str(), body.bodySize);

	std::cout << "******************** BODY ********************\n";
	for (size_t i = 0; i <= (int)body.bodySize; i++)
		std::cout << body.rawBody[i];
	std::cout << "\n**********************************************\n";
}

bool			Request::bufferContainChunk() {
	size_t			cpos = 0;
	size_t			crlfpos = 0;
	std::string		chunkSizeStr;

	crlfpos = buffer.find("\r\n");
	if (crlfpos == std::string::npos)
		return false;
	
	chunkSizeStr = buffer.substr(cpos, crlfpos - cpos);
	if (!isHexa(chunkSizeStr))
		return false;
	
	if (buffer.find("\r\n", crlfpos + 2) == std::string::npos)
		return false;
	return true;
}

bool			Request::decodeChunkedBody() {

	while (bufferContainChunk()) {
		// std::cout << "[INFO]\tBuffer Contain A Chunk..." << std::endl;
		std::string		chunkSizeStr;
		size_t			currPos = 0, CRLFpos = 0;
		
		CRLFpos = buffer.find("\r\n");
		if (CRLFpos == std::string::npos || CRLFpos == currPos)
			return false;
		
		chunkSizeStr = buffer.substr(currPos, CRLFpos - currPos);
		if (!isHexa(chunkSizeStr))
			return false;
		
		int chunkSize = hexToInt(chunkSizeStr);
		// std::cout << "=========================> [INFO]\tChunk Size = " << chunkSize << "..." << std::endl;
		if (!chunkSize && buffer.substr(CRLFpos, 4) == "\r\n\r\n") {
			// std::cout << "[INFO]\tReceived Last Chunk..." << std::endl;
			std::cout << "[INFO]\tTransfer Done..." << std::endl;
			buffer.erase(0, bufferSize);
			bufferSize = 0;
			pState = BODY_DECODED;
			return true;
		}

		currPos = CRLFpos + 2;
		if (bufferSize > currPos + chunkSize + 2)
			bufferSize -= currPos + chunkSize + 2;
		else {
			// std::cout << "[CHUNKS]\tNot Enough Data in Buffer" << std::endl;
			return true;
		}
		if (bufferSize < 0) {
			std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*> TMEJNINAAAAAAAAAAAAA" << bufferSize << "<-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
			std::cout << buffer << std::endl;
			std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*> TMEJNINAAAAAAAAAAAAA" << bufferSize << "<-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
			bufferSize = 0;
		}
		body.rawBody += buffer.substr(currPos, chunkSize);
		body.bodySize += chunkSize;
		// if (buffer.substr(currPos + chunkSize, 2) != "\r\n")
		// 	return false;
		buffer.erase(0, currPos + chunkSize + 2);
	}
	if (!bufferSize) {
		return false;
	}
	// std::cout << "[INFO]\tBuffer DOESN'T Contain A Chunk..." << std::endl;
	// std::cout << "WRITE TO DEBUG" << std::endl;
	// write(debugFD, "HelloDataRecv!\n", 15);
	// write(debugFD, buffer.c_str(), bufferSize);
	// write(debugFD, "----HelloDataRecv!\n", 19);
	// std::cout << "[INFO]\t------Buffer DOESN'T Contain A Chunk..." << std::endl;
	return true;
}

bool			Request::processNewPart() {
	size_t			currPos, filenamePos, filenamePos_;
	std::string		contentDisposition;
	std::string		filename;

	// std::cout << "processNewPart()" << std::endl;
	currPos = body.boundaryBegin.size() + 2;
	contentDisposition = body.rawBody.substr(currPos, body.rawBody.find("\r\n", currPos));
	filenamePos = contentDisposition.find("filename=\"") + 10;
	if (filenamePos == std::string::npos)
		return false;
	filenamePos_ = contentDisposition.find("\"", filenamePos);
	if (filenamePos_ == std::string::npos)
		return false;
	
	filename = contentDisposition.substr(filenamePos, filenamePos_ - filenamePos);
	if (filename.empty())
		return false;
	
	std::cout << filename << std::endl;
	std::string tmp = "/Users/tbesbess/goinfre/Webserv/" + filename;
	int	fd = open(tmp.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
	if (fd == -1)
		return false;
	
	files.push_back(fd);
	currPos = body.rawBody.find("\r\n\r\n", currPos);
	if (currPos == std::string::npos)
		return false;
	currPos += 4;
	body.rawBody.erase(0, currPos);
	body.bodySize -= currPos;
	return true;
}

bool			Request::processOldPart() {
	int				currentFile = files.back();
	size_t			bboundary = 0, eboundary = 0;

	// std::cout << "processOldPart()" << std::endl;
	// std::cout << "**************** RAW BODY ****************" << std::endl;
	// std::cout << body.rawBody << std::endl;
	// std::cout << "******************************************" << std::endl;
	bboundary = body.rawBody.find("\r\n" + body.boundaryBegin + "\r\n");
	eboundary = body.rawBody.find("\r\n" + body.boundaryEnd + "\r\n");
	
	int bytesToWrite = 0;
	if (bboundary == std::string::npos && eboundary == std::string::npos) {
		bytesToWrite = body.bodySize;
		write(currentFile, body.rawBody.c_str(), bytesToWrite);
		body.rawBody.clear(), body.bodySize = 0;
		return true;
	} else if (bboundary != std::string::npos)
		bytesToWrite = bboundary;
	else
		bytesToWrite = eboundary;
	
	int bytesWritten = write(currentFile, body.rawBody.c_str(), bytesToWrite);
	if (bytesWritten == -1)
		return false;
	
	body.rawBody.erase(0, bytesWritten + 2);
	body.bodySize -= bytesWritten + 2;
	if (body.rawBody == body.boundaryEnd + "\r\n")
		body.rawBody.clear(), body.bodySize = 0;
	return true;
}

bool			Request::processMultipartFormData() {
	if (body.rawBody.find(body.boundaryBegin + "\r\n") == 0 && body.rawBody.find("\r\n\r\n") != std::string::npos) {
		if (!processNewPart())
			return false;
	}
	if (body.rawBody.find(body.boundaryBegin + "\r\n") != 0 && files.size() > 0)
		processOldPart();
	return true;
}

bool			Request::processRequestRawBody() {
	while (!body.rawBody.empty()) {
		if (isMultipart) {
			processMultipartFormData();
		}
	}
	return true;
}

// BODY CONTROL CENTER
bool			Request::parseRequestBody() {
	if (!isEncoded)
		pState = BODY_DECODED;

	if (isEncoded && pState != BODY_DECODED)
		decodeChunkedBody();
	
	processRequestRawBody();
	// if (pState == BODY_DECODED)
	// {
	// }
	return false;
}