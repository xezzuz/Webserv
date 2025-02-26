/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 18:26:22 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/26 18:13:30 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void						Request::decodeURI() {
	std::string			encodedURI = _RequestData.URI;
	std::string			decodedURI;

	for (size_t i = 0; i < encodedURI.size(); i++) {
		if (encodedURI[i] == '%' && i + 2 < encodedURI.size() && isHexa(encodedURI.substr(i + 1, 2))) {
			decodedURI += htoi(encodedURI.substr(i + 1, 2));
			i += 2;
		} else if (encodedURI[i] == '+') {
			decodedURI += ' ';
		} else {
			decodedURI += encodedURI[i];
		}
	}
	_RequestData.URI = decodedURI;
}

void						Request::isValidMethod() {
	if (_RequestData.Method.empty())
		throw (400);
	
	if (_RequestData.Method != "GET" && _RequestData.Method != "POST" && _RequestData.Method != "DELETE")
		throw (501);
	
	if (std::find(_RequestData._Config->methods.begin(), _RequestData._Config->methods.end(), _RequestData.Method) == _RequestData._Config->methods.end())
		throw (405);
}

void						Request::isValidURI() {
	if (_RequestData.URI.empty() || _RequestData.URI[0] != '/')
		throw (400);
	
	if (_RequestData.URI.size() > 2048)
		throw (414);
	
	char allowedURIChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ._-~:/?#[]@!$&'()*+,;=%";
	if (_RequestData.URI.find_first_not_of(allowedURIChars) != std::string::npos)
		throw (400);
	
	decodeURI();
}

void						Request::isValidHTTPVersion() {
	if (_RequestData.HTTPversion.empty())
		throw (400);
	
	if (_RequestData.HTTPversion.size() != 8 || _RequestData.HTTPversion.find("HTTP/") != 0)
		throw (400);
	
	if (_RequestData.HTTPversion != "HTTP/1.1")
		throw (505);
}


void						Request::parseRequestLine() {
	size_t					CRLFpos = buffer.find(CRLF);

	_RequestRaws.rawRequestLine = buffer.substr(0, CRLFpos);
	buffer.erase(0, CRLFpos + 2);
	bufferSize -= CRLFpos;
	
	if (std::count(_RequestRaws.rawRequestLine.begin(), _RequestRaws.rawRequestLine.end(), ' ') != 2)
		throw (400);
	
	std::stringstream		RLss(_RequestRaws.rawRequestLine);

	std::getline(RLss, _RequestData.Method, ' ');
	std::getline(RLss, _RequestData.URI, ' ');
	std::getline(RLss, _RequestData.HTTPversion, ' ');
	
	isValidMethod();
	isValidURI();
	isValidHTTPVersion();
}

void						Request::parseRequestHeaders() {
	size_t					CRLFpos = buffer.find(DOUBLE_CRLF);
	std::stringstream		Hss(buffer.substr(0, CRLFpos + 4));
	buffer.erase(0, CRLFpos + 4);
	bufferSize -= CRLFpos;

	std::string		fieldline;
	char 			allowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'*+-.^_`|~";
	
	while (getline())
}

void						Request::validateRequestHeaders() {

}

// HEADERS CONTROL CENTER
void						Request::parseRequestLineAndHeaders() {
	parseRequestLine();
	parseRequestHeaders();
	validateRequestHeaders();
}
