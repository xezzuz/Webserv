/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2025/02/23 14:40:31 by mmaila           ###   ########.fr       */
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

# define RECV_BUFFER_SIZE 16384

enum e_reqState {
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
	int										bodySize;
	int										contentLength;
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
		bool							headersParsed;

		/*				STATE FLAGS				*/
		// e_parsingState					pState;
		// int								statusCode;
	
	public:
		Request(std::vector<ServerConfig>& vServers);
		~Request();
		Request(const Request& rhs);
		Request&	operator=(const Request& rhs);

		// int							feedRequest(int clientSocket);
		// bool						printParsedRequest();

		bool						bufferContainChunk();
		std::string					extractHeadersFromBuffer();
		// void						feedRequest(char *recvBuffer, int bufferSize);

		int							parseControlCenter(char *recvBuffer, int recvBufferSize);
		// bool						parseControlCenter();
		void						parseRequestLine();
		void						parseHeaders();
		bool						parseRequestBody();
		void						decodeChunkedBody();
		void						processRequestRawBody();
		void						processMultipartFormData();
		void						processMultipartHeaders();
		void						processMultipartData();
		bool						processBinaryBody();
		void						parseLengthBody();
		void						storeBody( void );
		void						uploadBody( void );
		
		void						validateRequestHeaders();

		// void						storeBody( void );
		// void						putRequestBodyInFile();

		std::string					getBuffer() const { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };
		RequestData					*getRequestData() { return &_RequestData; };

		bool						decodeURI();
		void						isValidMethod(const std::string& method);
		void						isValidURI(const std::string& uri);
		void						isValidHTTPVersion(const std::string& httpversion);
		
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