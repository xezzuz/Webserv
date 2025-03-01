/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:30:24 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/01 12:17:27 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// void	Request::storeBody( void )
// {
// 	uploader.open(std::tmpnam(NULL), std::ios::out | std::ios::trunc); // this line shouldnt happen everytime we try to write
// 	if (!uploader.is_open())
// 		throw(Code(500));

// 	uploader.write(_RequestRaws.rawBody.c_str(), _RequestRaws.rawBody.size());
// 	if (!uploader)
// 		throw(Code(500));
// 	_RequestRaws.rawBody.clear(); //
// }

// void			Request::putRequestBodyInFile() {
// 	int fd = open("RequestBody.txt", O_CREAT | O_RDWR | O_TRUNC);
// 	// std::tmpnam();
// 	write(fd, _RequestRaws.rawBody.c_str(), _RequestRaws.bodySize);

// 	std::cout << "******************** BODY ********************\n";
// 	for (int i = 0; i <= (int)_RequestRaws.bodySize; i++)
// 		std::cout << _RequestRaws.rawBody[i];
// 	std::cout << "\n**********************************************\n";
// }

bool			Request::bufferContainChunk() {
	size_t			cpos = 0;
	size_t			CRLFpos = 0;
	std::string		chunkSizeStr;

	CRLFpos = buffer.find(CRLF);
	if (CRLFpos == std::string::npos)
		return false;
	
	chunkSizeStr = buffer.substr(cpos, CRLFpos - cpos);
	if (!isHexa(chunkSizeStr))
		return false;
	
	if (buffer.find(CRLF, CRLFpos + 2) == std::string::npos)
		return false;
	return true;
}

void	Request::parseLengthBody() {
	if (!bufferSize)
		return ;
	_RequestRaws.rawBody += buffer.substr(0, bufferSize);
	_RequestRaws.bodySize += bufferSize;
	buffer.clear();
	bufferSize = 0;
	if (_RequestRaws.bodySize == _RequestData.contentLength)
		bodyFinished = true;
	else if (_RequestRaws.bodySize > _RequestData.contentLength)
		throw(Code(400));
}

// unchunks available chunks and send them to raw Body
void			Request::decodeChunkedBody() {

	while (bufferContainChunk()) {
		std::string		chunkSizeStr;
		int			currPos = 0, CRLFpos = 0;
		
		CRLFpos = buffer.find(CRLF);
		// if (CRLFpos == std::string::npos || CRLFpos == currPos)
		// 	return false;
		
		chunkSizeStr = buffer.substr(currPos, CRLFpos - currPos);
		if (!isHexa(chunkSizeStr))
			throw(Code(400));
		
		int chunkSize = htoi(chunkSizeStr);
		if (!chunkSize && buffer.substr(CRLFpos, 4) == DOUBLE_CRLF) {
			std::cout << "[INFO]\tBody Decoding is Done..." << std::endl;
			buffer.clear(), bufferSize = 0;
			bodyFinished = true;
			return ;
		} else if (!chunkSize && buffer.substr(CRLFpos, 4) != DOUBLE_CRLF)
			throw(Code(400));

		currPos = CRLFpos + 2;
		if (bufferSize <= currPos + chunkSize + 2)
			return ;

		_RequestRaws.rawBody += buffer.substr(currPos, chunkSize);
		_RequestRaws.bodySize += chunkSize;
		// if (buffer.substr(currPos + chunkSize, 2) != CRLF) // malformed chunk
		// 	return false;
		buffer.erase(0, currPos + chunkSize + 2);
		bufferSize -= currPos + chunkSize + 2;
	}
}

// new file, just create it i.e open the fileUploader filestream
void			Request::processMultipartHeaders() {
	size_t			currPos, filenamePos, filenamePos_;
	std::string		contentDisposition;
	std::string		filename;

	currPos = _RequestRaws.boundaryBegin.size() + 2;
	contentDisposition = _RequestRaws.rawBody.substr(currPos, _RequestRaws.rawBody.find(CRLF, currPos));
	filenamePos = contentDisposition.find("filename=\"") + 10;
	if (filenamePos == std::string::npos)
		throw Code(400);
	filenamePos_ = contentDisposition.find("\"", filenamePos);
	if (filenamePos_ == std::string::npos)
		throw (Code(400));
	
	filename = contentDisposition.substr(filenamePos, filenamePos_ - filenamePos);
	if (filename.empty())
		throw (Code(400));
	
	std::cout << "creating " + std::string(_RequestData._Config->upload_store + filename) << std::endl;
	fileUploader.open(std::string(_RequestData._Config->upload_store + filename).c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fileUploader.is_open())
		throw (Code(500));

	// std::string tmp = "/home/mmaila/goinfre/file.txx" + filename;
	// int	fd = open(tmp.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
	// if (fd == -1) {
	// 	throw (Code(500));
	// }
	// files.push_back(fd);
	
	currPos = _RequestRaws.rawBody.find(DOUBLE_CRLF, currPos);
	if (currPos == std::string::npos) {
		fileUploader.close();
		throw (Code(400));
	}
	currPos += 4;
	_RequestRaws.rawBody.erase(0, currPos);
	_RequestRaws.bodySize -= currPos;
}

// old file i.e write to it rawBody
void			Request::processMultipartData() {
	// int				currentFile = files.back();
	size_t			bboundary = 0, eboundary = 0;

	bboundary = _RequestRaws.rawBody.find(CRLF + _RequestRaws.boundaryBegin + CRLF);
	eboundary = _RequestRaws.rawBody.find(CRLF + _RequestRaws.boundaryEnd + CRLF);
	
	std::cout << "writing to old file" << std::endl;
	int bytesToWrite = 0;
	if (bboundary == std::string::npos && eboundary == std::string::npos) {
		bytesToWrite = _RequestRaws.bodySize;
		// int bytesWritten = write(currentFile, _RequestRaws.rawBody.c_str(), bytesToWrite);
		// if (bytesWritten == -1) {
		// 	printf("write");
		// 	throw(500);
		// }
		
		if (!fileUploader.is_open())
			throw (Code(500));
		
		fileUploader << _RequestRaws.rawBody; // should i flush after this?
		if (fileUploader.fail()) {
			fileUploader.close();
			throw (Code(500));
		}
		
		_RequestRaws.rawBody.clear();
		_RequestRaws.bodySize = 0;
		// _RequestRaws.rawBody.erase(0, bytesWritten), _RequestRaws.bodySize -= bytesWritten;
		return ;
	} else if (bboundary != std::string::npos)
		bytesToWrite = bboundary;
	else
		bytesToWrite = eboundary;
	
	fileUploader << _RequestRaws.rawBody.substr(0, bytesToWrite); // should i flush after this?
	if (fileUploader.fail()) {
		fileUploader.close();
		throw (Code(500));
	}

	
	// int bytesWritten = write(currentFile, _RequestRaws.rawBody.c_str(), bytesToWrite);
	// if (bytesWritten == -1)
	// 	throw(500);
	
	_RequestRaws.rawBody.erase(0, bytesToWrite + 2);
	_RequestRaws.bodySize -= bytesToWrite + 2;
	// if (_RequestRaws.rawBody == _RequestRaws.boundaryEnd + CRLF) // i moved this one to below
	// 	_RequestRaws.rawBody.clear(), _RequestRaws.bodySize = 0;
}

void			Request::processMultipartFormData() {
	while (!_RequestRaws.rawBody.empty()) {
		// if rawBody contain boundaryBegin and Headers CRLF-CRLF
		if (_RequestRaws.rawBody.find(_RequestRaws.boundaryBegin + CRLF) == 0 && _RequestRaws.rawBody.find(DOUBLE_CRLF) != std::string::npos) {
			// i.e new file
			std::cout << "[BODY]\tNEW FILE" << std::endl;
			processMultipartHeaders();
		}
		if (_RequestRaws.rawBody.find(_RequestRaws.boundaryBegin + CRLF) != 0 && fileUploader.is_open()) {
			// i.e old file
			std::cout << "[BODY]\tOLD FILE" << std::endl;
			processMultipartData();
		}
		if (_RequestRaws.rawBody == _RequestRaws.boundaryEnd + CRLF) // should i set bodyFinished?
			_RequestRaws.rawBody.clear(), _RequestRaws.bodySize = 0, fileUploader.close(), _RequestData.StatusCode = 201;
		// std::cout << "wa l7amaaaaaaa9 infinite loop hhhhhhh" << std::endl;
	}
}

void			Request::processBinaryBody() {
	if (_RequestRaws.rawBody.empty())
		return ;
	// if (_RequestRaws.bodySize > _RequestData.contentLength)
	//	throw(400);

	if (!fileUploader.is_open()) {
		std::string		fileUploaderRandname = "file_" + generateRandomString();
		
		if (access(fileUploaderRandname.c_str(), F_OK) == 0)
			throw (Code(503)); // 503 (service unavailable) or should we throw (Code(409)) conflict
		
		size_t			semiColonPos = _RequestData.contentType.find(';');
		
		std::string		contentType = semiColonPos != std::string::npos ? 
			_RequestData.contentType.substr(0, semiColonPos) : _RequestData.contentType;
		// std::string		contentType = semiColonPos != std::string::npos ? 
		// 	_RequestData.contentType.substr(0, semiColonPos) : contentType = _RequestData.contentType;
		
		if (_RequestRaws.mimeTypes.find(_RequestData.contentType) == _RequestRaws.mimeTypes.end())
			throw (Code(415));
		
		fileUploader.open(std::string(_RequestData._Config->upload_store + fileUploaderRandname 
			+ _RequestRaws.mimeTypes[contentType]).c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	}
	
	if (!fileUploader.is_open())
		throw (Code(500));
	
	fileUploader << _RequestRaws.rawBody;  // should i flush after this?
	if (fileUploader.fail()) {
		fileUploader.close();
		throw (Code(500));
	}
	
	_RequestRaws.rawBody.clear();
	_RequestRaws.bodySize = 0;

	if (bodyFinished)
		_RequestData.StatusCode = 201;
	
	// if (!files.size()) {
	// 	std::time_t				time = std::time(NULL);
	// 	std::stringstream 		ss;
	// 	ss << "/home/mmaila/goinfre/" << "_clientfile" << time;
	// 	std::string				filename(ss.str());
	// 	int fd = open(filename.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
	// 	if (fd == -1)
	// 		throw(500);
	// 	files.push_back(fd);
	// }
	// int bytesWritten = write(files.back(), _RequestRaws.rawBody.c_str(), _RequestRaws.bodySize);
	// if (bytesWritten == -1)
	// 	throw(500);
	// _RequestRaws.rawBody.erase(0, bytesWritten);
	// _RequestRaws.bodySize -= bytesWritten;
	// buffer.erase(0, bytesWritten);
	// _RequestRaws.bodySize -= bytesWritten;
}

void			Request::processRegularRequestRawBody() {
	// at this point the rawBody contains RAWBODY i.e decoded body (unchunked)
	
	// if (_RequestRaws.bodySize > _RequestData.contentLength) // i need to check this
	//	throw(Code(400));
	std::cout << "processRequestRawBody(1);" << std::endl;
	if (!_RequestRaws.bodySize) // i need to check this
		return ;
	std::cout << "processRequestRawBody(2);" << std::endl;
	
	if (isMultipart) {
		std::cout << "**************************** is multipart!" << std::endl;
		processMultipartFormData();
	} else {
		std::cout << "**************************** is binary!" << std::endl;
		processBinaryBody();
	}
	std::cout << "processRequestRawBody(true);" << std::endl;
}

void			Request::processCGIRequestRawBody() {
	// at this point the rawBody contains RAWBODY i.e decoded body (unchunked)
	char *CGITempFilename = std::tmpnam(NULL);
	if (!CGITempFilename)
		throw (Code(500));
	
	_RequestData.CGITempFilename = CGITempFilename;
	_RequestData.CGITempFilestream.open(_RequestData.CGITempFilename.c_str(), std::ios::out | std::ios::app | std::ios::binary); // append or trunc?
	if (!_RequestData.CGITempFilestream.is_open())
		throw (Code(500));
	
	_RequestData.CGITempFilestream << _RequestRaws.rawBody;  // should i flush after this?
	_RequestRaws.rawBody.clear();
	_RequestRaws.bodySize = 0;
	
	_RequestData.CGITempFilestream.close();
	if (_RequestData.CGITempFilestream.fail())
		throw (Code(500));
}

// BODY CONTROL CENTER
void			Request::parseRequestBody() {		// store request body in raw Body
	std::cout << "[REQUEST]\tParsing Body..." << std::endl;
	if (isEncoded)
		decodeChunkedBody(); // after this the decoded body will be stored in _RequestRaws.rawBody
	else
		parseLengthBody();   // after this the decoded body will be stored in _RequestRaws.rawBody
	
	if (_RequestData.isCGI)
		processCGIRequestRawBody();     // process _RequestRaws.rawBody
	else
		processRegularRequestRawBody(); // process _RequestRaws.rawBody
}
