/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:46:13 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/13 19:10:13 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}											t_header;

typedef struct								s_body {
	std::string								rawBody;
	std::string								boundaryBegin;
	std::string								boundaryEnd;
	size_t									bodySize;
	size_t									contentLength;
}											t_body;

class Request {
	private:
		std::vector<std::string>		rawRequest;
		size_t							statusCode;
		t_body							body;
		t_header						header;
		t_requestline					requestLine;
		Request();
	
	public:
		Request(std::string rawRequest);
		~Request();

		void						printParsedRequest();

		bool						parseRequest(const std::string& rawRequest);
		bool						parseRequestLine();
		bool						parseHeaders();
		bool						parseBody();

		std::vector<std::string>&	getRawRequest();
		t_body&						getBodySt();
		t_header&					getHeaderSt();
		t_requestline&				getRequestLineSt();
};

std::string		stringtrim(const std::string& str);
std::string		stringtolower(std::string& str);