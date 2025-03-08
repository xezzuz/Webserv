/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 20:11:27 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/08 01:53:12 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientHandler.hpp"

void						produceAbsPath(RequestData& _RequestData) {
	
	if (!_RequestData._Config->redirect.second.empty())
	{
		if (_RequestData._Config->redirect.second.find("http://") != std::string::npos
			|| _RequestData._Config->redirect.second.find("https://") != std::string::npos)
		{
			throw(Code(_RequestData._Config->redirect.first, _RequestData._Config->redirect.second));
		}
		else
		{
			if (_RequestData._Config->redirect.second.at(0) != '/')
				_RequestData._Config->redirect.second.insert(0, "/");
			throw(Code(_RequestData._Config->redirect.first, "http://" + _RequestData.host + _RequestData._Config->redirect.second));
		}
	}

	if (!_RequestData._Config->alias.empty()) {
		if (_RequestData._Config->alias[_RequestData._Config->alias.length() - 1] == '/')
			_RequestData._Config->alias.erase(_RequestData._Config->alias.length() - 1);
		_RequestData.fullPath = _RequestData._Config->alias + _RequestData.URI.substr(_RequestData.matchingLocation.length());
	}
	else {
		if (_RequestData._Config->root[_RequestData._Config->root.length() - 1] == '/')
			_RequestData._Config->root.erase(_RequestData._Config->root.length() - 1);
		_RequestData.fullPath = _RequestData._Config->root + _RequestData.URI;
	}
}

void						setQueryString(RequestData& _RequestData) {
	size_t		pos;
	std::string	query;

	pos = _RequestData.fullPath.find_last_of('#');
	if (pos != std::string::npos)
		_RequestData.fullPath.erase(pos);
	
	pos = _RequestData.fullPath.find_first_of('?');
	if (pos != std::string::npos)
	{
		_RequestData.queryString = _RequestData.fullPath.substr(pos + 1);
		_RequestData.fullPath.erase(pos);
	}
}

void						setRequestedResourceType(RequestData& _RequestData) {
	struct stat	pathStats;
	std::string	pathChecker;
	
	size_t	startPos = 0;
	size_t	endPos = 0;
	while (startPos < _RequestData.fullPath.size()) {
		endPos = _RequestData.fullPath.find('/', startPos + 1);
		pathChecker.append(_RequestData.fullPath.substr(startPos, endPos - startPos));
		if (stat(pathChecker.c_str(), &pathStats) != 0)
			throw(Code(404));
		if (!S_ISDIR(pathStats.st_mode))
			break ;
		startPos = endPos;
	}
	_RequestData.isDir = S_ISDIR(pathStats.st_mode) ? true : false;
	std::string pathInfo = _RequestData.fullPath.substr(pathChecker.size());
	if (!pathInfo.empty())
		_RequestData.scriptName = _RequestData.URI.substr(0, _RequestData.URI.find(pathInfo));
	else
		_RequestData.scriptName = _RequestData.URI.substr(0, _RequestData.URI.find_first_of("?"));
	_RequestData.pathTranslated = _RequestData.fullPath;
	_RequestData.pathInfo = _RequestData.URI.substr(0, _RequestData.URI.find_first_of('?'));
	_RequestData.fullPath.erase(pathChecker.size());
}

void						handleDirectoryResource(RequestData& _RequestData) {
	if (access(_RequestData.fullPath.c_str(), X_OK) != 0)
		throw(Code(403));
	
	if (_RequestData.fullPath.at(_RequestData.fullPath.size() - 1) != '/')
		_RequestData.fullPath.append("/");

	if (_RequestData.Method == "GET") {
		std::vector<std::string>::iterator	it = _RequestData._Config->index.begin();
		
		for (; it != _RequestData._Config->index.end(); it++) {
			if (it->at(0) == '/')
				it->erase(0, 1);
			if (access((_RequestData.fullPath + (*it)).c_str(), F_OK) == 0) {
				_RequestData.fullPath += *it;
				_RequestData.isDir = false;
				break ;
			}
		}
		
		if (it == _RequestData._Config->index.end()) {
			if (!_RequestData._Config->autoindex)
				throw(Code(403));
		}
	}
}

bool						extensionIsCGI(RequestData& _RequestData) {
	size_t dot = _RequestData.fullPath.find_last_of('.');
	if (dot == std::string::npos)
		return (false);

	std::string file_ext = _RequestData.fullPath.substr(dot);
	
	std::map<std::string, std::string>::iterator it = _RequestData._Config->cgi_ext.find(file_ext);
	if (it != _RequestData._Config->cgi_ext.end())
	{
		_RequestData.cgiIntrepreter = it->second;
		return true;
	}
	return false;
}

void						handleFileResource(RequestData& _RequestData) {
	if (access(_RequestData.fullPath.c_str(), R_OK) != 0)
		throw(Code(403));

	_RequestData.fileName = _RequestData.fullPath.substr(_RequestData.fullPath.find_last_of('/') + 1);

	_RequestData.isCGI = extensionIsCGI(_RequestData);
	if (!_RequestData.isCGI && _RequestData.scriptName != _RequestData.pathInfo)
		throw(Code(404));
}

void						resolveAbsPath(RequestData& _RequestData) {

	setQueryString(_RequestData);
	setRequestedResourceType(_RequestData);
	if (_RequestData.isDir)
		handleDirectoryResource(_RequestData);
	if (!_RequestData.isDir)
		handleFileResource(_RequestData);
}

void						resolveURI(RequestData& _RequestData) {
	_RequestData.fullPath.clear();

	produceAbsPath(_RequestData);
	resolveAbsPath(_RequestData);
	if (_RequestData.isDir && _RequestData.URI[_RequestData.URI.size() - 1] != '/')
		throw(Code(308, "http://" + _RequestData.host + _RequestData.URI + '/'));

	if (_RequestData.Method == "POST")
	{
		if (!_RequestData.isCGI)
		{
			if (_RequestData._Config->upload_store.empty())
				throw (Code(403));
			else if (!_RequestData.isDir)
				throw (Code(405));
		}
		if (_RequestData.contentLength > _RequestData._Config->client_max_body_size)
			throw (Code(413));
	}
}
