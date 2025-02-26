/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/26 17:58:13 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <iostream>
# include <iomanip>
# include <sstream>
# include <string.h>
# include <map>
# include <vector>
# include <algorithm>

# include <fcntl.h>
# include <sys/socket.h>
# include <unistd.h>
# include "../../Utils/Helpers.hpp"
# include "../../_Config/Config.hpp"

# define RECV_BUFFER_SIZE 16384
# define CRLF "\r\n"
# define DOUBLE_CRLF "\r\n\r\n"

enum e_parsingState {
	RECV,
	RESPOND,
	FORWARD_CGI
};

enum e_reqType {
	REGULAR,
	CGI
};

typedef struct								RequestData {
	/*					  BOOLEANS  				*/
	bool									isCGI;
	bool									isDir;
	bool									isRange;
	bool									keepAlive;
	
	int										StatusCode;
	
	/*					REQUEST LINE				*/
	std::string								Method;
	std::string								URI;
	std::string								HTTPversion;
	
	/*						CGI						*/
	std::string								queryString;
	std::string								pathInfo;
	std::string								scriptName;
	std::string								cgiIntrepreter;
	
	/*					  RESPONSE   				*/
	std::string								fullPath;
	std::string								contentType;
	size_t									contentLength;
	std::string								connection;
	std::string								host;
	std::string								transferEncoding;
	std::string								matchingLocation;

	std::map<std::string, std::string>		Headers;
	Directives								*_Config; // ptr
	
	RequestData() : isCGI(false), isDir(false), isRange(false), keepAlive(true), StatusCode(200) {}
}											RequestData;

typedef	struct								RequestRaws {
	std::string								rawRequestLine;
	std::vector<std::string>				rawHeaders;
	std::string								rawBody;
	
	std::string								boundaryBegin;
	std::string								boundaryEnd;
	size_t									bodySize;
}											RequestRaws;


class Request {
	private:
		/*			 PARSING BUFFRER			*/
		std::string						buffer;
		int								bufferSize;

		std::vector<int>				files;
		std::ofstream					uploader;

		/*			PARSING STRUCTURES			*/
		RequestData						_RequestData;
		RequestRaws						_RequestRaws;

		std::vector<ServerConfig>&		vServers;

		/*			   PARSING FLAGS			*/
		bool							isEncoded;
		bool							isMultipart;
		// bool							headersParsed;
		// bool							bodyDone;

		/*				STATE FLAGS				*/
		bool							headersFinished;
		bool							bodyFinished;
		e_parsingState					RequestState;
		// int								statusCode;
	
	public:
		Request(std::vector<ServerConfig>& vServers);
		~Request();
		Request(const Request& rhs);
		Request&	operator=(const Request& rhs);

		int							feedRequest(char *recvBuffer, int recvBufferSize);
		// bool						printParsedRequest();

		bool						bufferContainChunk();
		std::string					extractHeadersFromBuffer();
		// void						feedRequest(char *recvBuffer, int bufferSize);

		int							parseControlCenter(char *recvBuffer, int recvBufferSize);
		void						parseRequestLineAndHeaders();
		void						parseRequestLine();
		void						parseRequestHeaders();
		void						parseRequestBody();
		void						decodeChunkedBody();
		void						processRequestRawBody();
		void						processMultipartFormData();
		void						processMultipartHeaders();
		void						processMultipartData();
		void						processBinaryBody();
		void						parseLengthBody();
		void						storeBody( void );
		void						uploadBody( void );
		
		void						validateRequestHeaders();

		// void						putRequestBodyInFile();

		std::string					getBuffer() const { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };
		RequestData					*getRequestData() { return &_RequestData; };

		void						decodeURI();
		void						isValidMethod();
		void						isValidURI();
		void						isValidHTTPVersion();
		
		void						setMatchingConfig();
		ServerConfig&				getMatchingServer();
};

void			fillRequestData(const std::string URI, RequestData& _RequestData);

bool			stringIsDigit(const std::string& str);
std::string		stringtrim(const std::string& str, const std::string& set);
std::string		stringtolower(std::string str);
bool			isHexa(const std::string& num);

#endif