/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/08 21:45:54 by mmaila           ###   ########.fr       */
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

# include <fcntl.h>
# include <sys/socket.h>
# include <unistd.h>
# include "../../Utils/Helpers.hpp"
# include "../../_Config/Config.hpp"

# define REQUEST_BUFFER_SIZE 16000

enum e_parsingState {
	REQUEST_INIT,
	REQUEST_HEADERS,
	REQUEST_FINISHED
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
	int										bodySize;
	int										contentLength;
}											RequestRaws;


class Request {
	private:
		/*			 PARSING BUFFRER			*/
		std::string						buffer;
		int								bufferSize;

		std::vector<int>				files;

		/*			PARSING STRUCTURES			*/
		RequestData						_RequestData;
		RequestRaws						_RequestRaws;

		std::vector<ServerConfig>&		vServers;

		/*			   PARSING FLAGS			*/
		bool							isEncoded;
		bool							isMultipart;

		/*				STATE FLAGS				*/
		e_parsingState					pState;
		// int								statusCode;
	
	public:
		Request(std::vector<ServerConfig>& vServers);
		~Request();
		Request(const Request& rhs);
		Request&	operator=(const Request& rhs);

		int							feedRequest(int clientSocket);
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
		bool						processMultipartHeaders();
		bool						processMultipartData();
		bool						processBinaryBody();
		bool						parseLengthBody();
		bool						validateRequestHeaders();

		void						putRequestBodyInFile();

		std::string&				getBuffer() { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };
		RequestData					*getRequestData() { return &_RequestData; };

		int&						getStatusCode();
		e_parsingState				getParsingState() { return pState; } ;

		void						setStatusCode(int code);
		bool						decodeURI();
		bool						isValidFieldLine(const std::string& fieldline);
		bool						isValidMethod(const std::string& method);
		bool						isValidURI(const std::string& uri);
		bool						isValidHTTPVersion(const std::string& httpversion);
		
		void						setMatchingConfig();
		ServerConfig&				getMatchingServer();
};

void			fillRequestData(const std::string URI, RequestData& _RequestData);

bool			stringIsDigit(const std::string& str);
std::string		stringtrim(const std::string& str, const std::string& set);
std::string		stringtolower(std::string str);
bool			isHexa(const std::string& num);
int				hexToInt(const std::string& num);

#endif