/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2025/03/07 19:25:57 by mmaila           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sstream>
# include <string.h>
# include <map>
# include <vector>
# include "../../Utils/Helpers.hpp"
# include "../../_Config/Config.hpp"

# define RECV_BUFFER_SIZE 16384

# ifndef CRLF
# define CRLF "\r\n"
# endif
# ifndef DOUBLE_CRLF
# define DOUBLE_CRLF "\r\n\r\n"
# endif

enum e_parsingState {
	RECV,			// 0
	RESPOND,		// 1
	FORWARD_CGI		// 2
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
	bool									isEncoded;
	bool									isMultipart;
	
	int										StatusCode;
	
	/*					REQUEST LINE				*/
	std::string								Method;
	std::string								URI;
	std::string								HTTPversion;
	
	/*						CGI						*/
	std::string								queryString;
	std::string								pathInfo;
	std::string								scriptName;
	std::string								fileName;
	std::string								pathTranslated;
	std::string								cgiIntrepreter;
	std::string								serverHost;
	std::string								serverPort;
	std::string								CGITempFilename;
	
	/*					  RESPONSE   				*/
	std::string								fullPath;
	std::string								contentType;
	size_t									contentLength;
	std::string								connection;
	std::string								host;
	std::string								transferEncoding;
	std::string								matchingLocation;

	std::map<std::string, std::string>		Headers;
	Directives								*_Config;
	
	RequestData() : isCGI(false), isDir(false), isRange(false), keepAlive(true), isEncoded(false), isMultipart(false), StatusCode(200), contentLength(0) {}
}											RequestData;

typedef	struct								RequestRaws {
	std::string								rawRequestLine;
	std::vector<std::string>				rawHeaders;
	std::string								rawBody;
	
	std::string								boundaryBegin;
	std::string								boundaryEnd;
	size_t									rawBodySize;
	size_t									totalBodySize;

	std::map<std::string, std::string>		mimeTypes;

	RequestRaws() : rawBodySize(0), totalBodySize(0) {}
}											RequestRaws;


class Request {
	private:
		/*			 PARSING BUFFRER			*/
		std::string						buffer;

		std::ofstream					fileUploader;

		/*			PARSING STRUCTURES			*/
		RequestData						_RequestData;
		RequestRaws						_RequestRaws;

		std::vector<ServerConfig>&		vServers;

		/*				STATE FLAGS				*/
		bool							headersFinished;
		bool							bodyFinished;
		e_parsingState					RequestState;
	
	public:
		Request(std::vector<ServerConfig>& vServers);
		Request(const Request& rhs);
		Request&	operator=(const Request& rhs);
		~Request();

		bool						bufferContainChunk();

		int							parseControlCenter(char *recvBuffer, int recvBufferSize);
		void						parseRequestLineAndHeaders();
		void						parseRequestLine();
		void						parseRequestHeaders();
		void						validateRequestHeaders();
		void						parseRequestBody();
		void						decodeChunkedBody();
		void						parseLengthBody();
		void						processRegularRequestRawBody();
		void						processCGIRequestRawBody();
		void						processMultipartFormData();
		void						processMultipartHeaders();
		void						processMultipartData();
		void						processBinaryBody();
		

		std::string					getBuffer() const { return buffer; };
		void						setBuffer(const std::string& newValue) { this->buffer = newValue; };
		void						setFullPath(const std::string& path) { _RequestData.fullPath = path; }
		RequestData					*getRequestData() { return &_RequestData; };

		bool						headerExists(const std::string& key);
		bool						isCriticalHeader(const std::string& key);
		void						resolveURITraversal(std::string& URI);
		void						decodeURI();
		void						isValidMethod();
		void						isValidURI();
		void						isValidHTTPVersion();

		void						setupCGITempFile();
		
		void						setMatchingConfig();
		ServerConfig&				getMatchingServer();
};

void			resolveURI(RequestData& _RequestData);
void			resolveAbsPath(RequestData& _RequestData);

bool			stringisdigit(const std::string& str);
std::string		stringtrim(const std::string& str, const std::string& set);
std::string		stringtolower(std::string str);
bool			isHexa(const std::string& num);

#endif