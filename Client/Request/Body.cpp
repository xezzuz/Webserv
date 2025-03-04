/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:30:24 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/04 02:37:02 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

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
	if (buffer.empty())
		return ;

	std::cout << RED << "totalBodySize = " << _RequestRaws.totalBodySize << RESET << std::endl;
	std::cout << RED << "buffer size = " << buffer.size() << RESET << std::endl;
	std::cout << RED << "clientmaxbodysize = " << _RequestData._Config->client_max_body_size << RESET << std::endl;
	std::cout << RED << "content length = " << _RequestData.contentLength << RESET << std::endl;

	if (_RequestRaws.totalBodySize + buffer.size() < _RequestRaws.totalBodySize)
		throw Code(413);
	if (_RequestRaws.totalBodySize + buffer.size() > _RequestData._Config->client_max_body_size)
		throw Code(413);
	if (_RequestRaws.totalBodySize + buffer.size() > _RequestData.contentLength)
		throw Code(400);

	_RequestRaws.rawBody.append(buffer);
	_RequestRaws.rawBodySize += buffer.size();
	_RequestRaws.totalBodySize += buffer.size();
	buffer.clear();
	
	if (_RequestRaws.totalBodySize == _RequestData.contentLength)
		bodyFinished = true;
	
}

// unchunks available chunks and send them to raw Body
void			Request::decodeChunkedBody() {

	while (bufferContainChunk()) {
		std::string		chunkSizeStr;
		size_t			currPos = 0, CRLFpos = 0;
		
		CRLFpos = buffer.find(CRLF);
		// if (CRLFpos == std::string::npos || CRLFpos == currPos)
		// 	return false;
		
		chunkSizeStr = buffer.substr(currPos, CRLFpos - currPos);
		if (!isHexa(chunkSizeStr))
			throw Code(400);
		
		size_t chunkSize = htoi(chunkSizeStr);
		if (!chunkSize && buffer.substr(CRLFpos, 4) == DOUBLE_CRLF) {
			std::cout << "[INFO]\tBody Decoding is Done..." << std::endl;
			buffer.clear();
			bodyFinished = true;
			return ;
		} else if (!chunkSize && buffer.substr(CRLFpos, 4) != DOUBLE_CRLF)
			throw Code(400);

		currPos = CRLFpos + 2;
		if (buffer.size() <= currPos + chunkSize + 2)
			return ;

		if (_RequestRaws.totalBodySize + chunkSize < _RequestRaws.totalBodySize)
			throw Code(413);
		if (_RequestRaws.totalBodySize + chunkSize > _RequestData._Config->client_max_body_size)
			throw Code(413);

		_RequestRaws.rawBody += buffer.substr(currPos, chunkSize);
		_RequestRaws.rawBodySize += chunkSize;
		_RequestRaws.totalBodySize += chunkSize;
		// if (buffer.substr(currPos + chunkSize, 2) != CRLF) // malformed chunk
		// 	return false;
		buffer.erase(0, currPos + chunkSize + 2);
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
		throw Code(400);
	
	filename = contentDisposition.substr(filenamePos, filenamePos_ - filenamePos);
	if (filename.empty())
		throw Code(400);
	
	std::cout << "creating " + std::string(_RequestData._Config->upload_store + filename) << std::endl;
	fileUploader.open(std::string(_RequestData._Config->upload_store + filename).c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fileUploader.is_open())
		throw Code(500);
	
	currPos = _RequestRaws.rawBody.find(DOUBLE_CRLF, currPos);
	if (currPos == std::string::npos) {
		fileUploader.close();
		throw Code(400);
	}
	currPos += 4;
	_RequestRaws.rawBody.erase(0, currPos);
	_RequestRaws.rawBodySize -= currPos;
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
		bytesToWrite = _RequestRaws.rawBodySize;
		
		if (!fileUploader.is_open())
			throw Code(500);
		
		// fileUploader << _RequestRaws.rawBody; // should i flush after this?
		fileUploader.write(_RequestRaws.rawBody.c_str(), _RequestRaws.rawBody.size());
		if (fileUploader.fail()) {
			fileUploader.close();
			throw Code(500);
		}
		
		_RequestRaws.rawBody.clear();
		_RequestRaws.rawBodySize = 0;
		// _RequestRaws.rawBody.erase(0, bytesWritten), _RequestRaws.rawBodySize -= bytesWritten;
		return ;
	} else if (bboundary != std::string::npos)
	bytesToWrite = bboundary;
	else
	bytesToWrite = eboundary;
	
	// fileUploader << _RequestRaws.rawBody.substr(0, bytesToWrite); // should i flush after this?
	fileUploader.write(_RequestRaws.rawBody.substr(0, bytesToWrite).c_str(), bytesToWrite);
	if (fileUploader.fail()) {
		fileUploader.close();
		throw Code(500);
	}

	
	// int bytesWritten = write(currentFile, _RequestRaws.rawBody.c_str(), bytesToWrite);
	// if (bytesWritten == -1)
	// 	throw(500);
	
	_RequestRaws.rawBody.erase(0, bytesToWrite + 2);
	_RequestRaws.rawBodySize -= bytesToWrite + 2;
	// if (_RequestRaws.rawBody == _RequestRaws.boundaryEnd + CRLF) // i moved this one to below
	// 	_RequestRaws.rawBody.clear(), _RequestRaws.rawBodySize = 0;
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
			_RequestRaws.rawBody.clear(), _RequestRaws.rawBodySize = 0, fileUploader.close(), _RequestData.StatusCode = 201;
		// std::cout << "wa l7amaaaaaaa9 infinite loop hhhhhhh" << std::endl;
	}
}

void			Request::processBinaryBody() {
	if (_RequestRaws.rawBody.empty())
		return ;
	// if (_RequestRaws.rawBodySize > _RequestData.contentLength)
	//	throw(400);

	if (!fileUploader.is_open()) {
		std::string		fileUploaderRandname = "file_" + generateRandomString();
		
		if (access(fileUploaderRandname.c_str(), F_OK) == 0)
			throw Code(503); // 503 (service unavailable) or should we throw (Code(409)) conflict
		
		size_t			semiColonPos = _RequestData.contentType.find(';');
		
		std::string		contentType = semiColonPos != std::string::npos ? 
			_RequestData.contentType.substr(0, semiColonPos) : _RequestData.contentType;
		
		if (_RequestRaws.mimeTypes.find(_RequestData.contentType) == _RequestRaws.mimeTypes.end())
			throw Code(415);
		
		fileUploader.open(std::string(_RequestData._Config->upload_store + fileUploaderRandname 
			+ _RequestRaws.mimeTypes[contentType]).c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	}
	
	if (!fileUploader.is_open())
		throw Code(500);
	
	fileUploader.write(_RequestRaws.rawBody.c_str(), _RequestRaws.rawBodySize);
	if (fileUploader.fail()) {
		fileUploader.close();
		throw Code(500);
	}
	
	_RequestRaws.rawBody.clear();
	_RequestRaws.rawBodySize = 0;

	if (bodyFinished) {
		fileUploader.close();
		_RequestData.StatusCode = 201;
	}
}

void			Request::processRegularRequestRawBody() {
	// at this point the rawBody contains RAWBODY i.e decoded body (unchunked)
	
	// if (_RequestRaws.rawBodySize > _RequestData.contentLength) // i need to check this
	//	throw(Code(400));
	std::cout << "processRequestRawBody(1);" << std::endl;
	if (!_RequestRaws.rawBodySize) // i need to check this
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

void	Request::setupCGITempFile() {
	char *tmpName = std::tmpnam(NULL);
	if (!tmpName)
		throw Code(500);
	
	_RequestData.CGITempFilename = tmpName;
	fileUploader.open(_RequestData.CGITempFilename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!fileUploader.is_open())
		throw Code(500);
}

void			Request::processCGIRequestRawBody() {
	std::cout << "processCGIRequestRawBody();" << std::endl;
	std::cout << "rawBodySize = " << _RequestRaws.rawBodySize << std::endl;
	// at this point the rawBody contains RAWBODY i.e decoded body (unchunked)
	fileUploader.write(_RequestRaws.rawBody.c_str(), _RequestRaws.rawBodySize);
	if (fileUploader.fail()) {
		fileUploader.close();
		throw Code(500);
	}

	_RequestRaws.rawBody.clear();
	_RequestRaws.rawBodySize = 0;
	if (bodyFinished)
		fileUploader.close();
}

// BODY CONTROL CENTER
void			Request::parseRequestBody() {
	// if (!_RequestData.isCGI && _RequestData._Config->upload_store.empty())
	// 	throw (Code(403));
	
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
