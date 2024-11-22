/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/22 18:18:30 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <map>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

enum e_parsingState {
	PARSING_INIT,
	HEADERS_RECEIVED,
	HEADERS_FINISHED,
	BODY_RECEIVED,
	BODY_FINISHED,
	PARSING_FINISHED
};

typedef struct								s_requestline {
	std::string								rawRequestLine;
	std::string								method;
	std::string								uri;
	std::string								httpversion;
	// std::string								query;
}											t_requestline;

typedef struct								s_header {
	std::vector<std::string>				rawHeaders;
	std::map<std::string, std::string>		headersMap;
	std::string								host;
	std::string								contentType;
	std::string								connection;
	std::string								transferEncoding;
	// std::string								contentLength;
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
		int								rawBodyFD;
		int								bufferFD;
		int								debugFD;
		/*			 PARSING BUFFRER			*/
		std::string						buffer;
		int								bufferSize;

		std::vector<int>				files;

		/*			PARSING STRUCTURES			*/
		t_body							body;
		t_header						header;
		t_requestline					requestLine;

		/*			   PARSING FLAGS			*/
		bool							isEncoded;
		bool							isMultipart;

		/*				STATE FLAGS				*/
		e_parsingState					pState;
		size_t							statusCode;
	
	public:
		Request();
		// Request(const std::string& rawRequest);
		~Request();

		bool						printParsedRequest();

		bool						bufferContainHeaders();
		bool						bufferContainChunk();
		std::string					extractHeadersFromBuffer();
		bool						headerExists(const std::string& key);
		void						feedRequest(char *recvBuffer, int bufferSize);

		bool						parseControlCenter();
		void						setRequestState();
		bool						parseRequestLineAndHeaders();
		bool						storeHeadersInVector();
		bool						parseRequestLine();
		bool						parseHeaders();
		bool						parseRequestBody();
		bool						decodeChunkedBody();
		bool						processRequestRawBody();
		bool						processMultipartFormData();
		bool						processNewPart();
		bool						processOldPart();
		bool						parseLengthBody();
		bool						parseContentType();
		bool						parseMultipartFormData();
		bool						bodyHasHeaders();
		bool						parseFormUrlEncoded();
		bool						validateRequestHeaders();

		void						putRequestBodyInFile();

		std::string&				getBuffer() { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };

		std::vector<std::string>&	getRawRequest();
		t_body&						getBodySt();
		t_header&					getHeaderSt();
		t_requestline&				getRequestLineSt();
		e_parsingState				getParsingState() { return pState; } ;

		void						setStatusCode(int code);
		// bool						getHeadersParsed() { return headersParsed; };
		// bool						getBodyParsed() { return bodyParsed; };
		// bool						getParsingFinished() { return parsingFinished; };
		bool						isValidFieldLine(const std::string& fieldline);
};

bool			stringIsDigit(const std::string& str);
std::string		stringtrim(const std::string& str, const std::string& set);
std::string		stringtolower(std::string& str);
bool			isHexa(const std::string& num);
int				hexToInt(const std::string& num);

#endif