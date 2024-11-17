/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/17 18:57:15 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <map>
#include <vector>

typedef struct								s_requestline {
	std::string								rawRequestLine;
	std::string								method;
	std::string								uri;
	std::string								httpversion;
	// std::string								query;
}											t_requestline;

typedef struct								s_header {
	std::string								rawHeader;
	std::map<std::string, std::string>		headersMap;
	std::string								host;
	std::string								contentType;
	std::string								connection;
	std::string								transferEncoding;
	std::string								contentLength;
}											t_header;

typedef struct								s_body {
	std::string								rawBody;
	std::string								boundaryBegin;
	std::string								boundaryEnd;
	int										bodySize;
	int										contentLength;
}											t_body;

class Request {
	private:
		std::string						buffer;
		std::vector<std::string>		rawRequest;
		size_t							statusCode;
		t_body							body;
		t_header						header;
		t_requestline					requestLine;

		bool							parsingFinished;
		bool							headersParsed;
		bool							bodyParsed;
	
	public:
		Request();
		Request(const std::string& rawRequest);
		~Request();

		bool						printParsedRequest();

		bool						headersReceived();
		bool						chunkReceived();
		std::string					extractHeadersFromBuffer();
		bool						headerExists(const std::string& key);

		bool						requestParseControl();
		bool						parseRequestLineAndHeaders();
		bool						storeHeadersInVector();
		bool						parseRequestLine();
		bool						parseHeaders();
		bool						parseRequestBody();
		bool						parseChunkedBody();
		bool						parseLengthBody();

		std::string&				getBuffer() { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };

		std::vector<std::string>&	getRawRequest();
		t_body&						getBodySt();
		t_header&					getHeaderSt();
		t_requestline&				getRequestLineSt();

		bool						getHeadersParsed() { return headersParsed; };
		bool						getBodyParsed() { return bodyParsed; };
		bool						getParsingFinished() { return parsingFinished; };
};

bool			stringIsDigit(const std::string& str);
std::string		stringtrim(const std::string& str);
std::string		stringtolower(std::string& str);
bool			isHexa(const std::string& num);
int				hexToInt(const std::string& num);