/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:33:50 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/07 00:29:22 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(std::vector<ServerConfig>&	vServers) : vServers(vServers) {
	headersFinished = false;
	bodyFinished = false;
	
	_RequestData._Config = &vServers[0].ServerDirectives;
	
	_RequestRaws.mimeTypes["text/plain"] = "";
	_RequestRaws.mimeTypes["text/html"] = ".html";
	_RequestRaws.mimeTypes["text/css"] = ".css";
	_RequestRaws.mimeTypes["text/csv"] = ".csv";
	_RequestRaws.mimeTypes["application/doc"] = ".doc";
	_RequestRaws.mimeTypes["image/gif"] = ".gif";
	_RequestRaws.mimeTypes["image/jpeg"] = ".jpg";
	_RequestRaws.mimeTypes["text/javascript"] = ".js";
	_RequestRaws.mimeTypes["application/json"] = ".json";
	_RequestRaws.mimeTypes["application/java-archive"] = ".jar";
	_RequestRaws.mimeTypes["audio/mpeg"] = ".mp3";
	_RequestRaws.mimeTypes["video/mp4"] = ".mp4";
	_RequestRaws.mimeTypes["video/mpeg"] = ".mpeg";
	_RequestRaws.mimeTypes["image/png"] = ".png";
	_RequestRaws.mimeTypes["application/pdf"] = ".pdf";
	_RequestRaws.mimeTypes["application/x-sh"] = ".sh";
	_RequestRaws.mimeTypes["audio/wav"] = ".wav";
	_RequestRaws.mimeTypes["audio/webm"] = ".weba";
	_RequestRaws.mimeTypes["video/webm"] = ".webm";
	_RequestRaws.mimeTypes["image/webp"] = ".webp";
	_RequestRaws.mimeTypes["application/xml"] = ".xml";
	_RequestRaws.mimeTypes["application/zip"] = ".zip";
	_RequestRaws.mimeTypes["application/x-tar"] = ".tar";
	_RequestRaws.mimeTypes["application/octet-stream"] = ".bin";
	_RequestRaws.mimeTypes["image/avif"] = ".avif";
	_RequestRaws.mimeTypes["video/x-msvideo"] = ".avi";
}

Request::Request(const Request& rhs) : vServers(rhs.vServers) {
	*this = rhs;
}

Request&	Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		buffer = rhs.buffer;
		
		_RequestData = rhs._RequestData;
		_RequestRaws = rhs._RequestRaws;
		
		headersFinished = rhs.headersFinished;
		bodyFinished = rhs.bodyFinished;
	}
	return *this;
}

Request::~Request() {
	if (fileUploader.is_open())
		fileUploader.close();
}