/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 17:54:07 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/02 19:33:54 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/stat.h>
#include "../Request/Request.hpp"
#include "../Config/Config.hpp"

// enum e_responseState {
// 	PARSING_INIT,		// 0
// 	HEADERS_RECEIVED,	// 1
// 	HEADERS_FINISHED,	// 2
// 	BODY_RECEIVED,		// 3
// 	BODY_FINISHED,		// 4
// 	PARSING_FINISHED	// 5
// };

typedef struct								s_statusline {
	std::string								httpversion;
	std::string								status_code;
	std::string								reason_phrase;
}											t_statusline;

class Response {
	private:
		bool								isReady;
		int									statusCode;
		
		std::string							requestedResource;

		Request*							_Request;
		ServerConfig						_Config;
		std::vector<ServerConfig>			vServerConfigs;

		LocationConfig						*locationBlock;

		t_statusline						statusLine;

		std::string							rawResponse;
		
	public:
		Response();
		Response(const Response& original);
		Response&	operator=(const Response& original);
		~Response();

		void				feedResponse(Request* _Request);
		bool				generateResponse();
		void				setMatchingLocationBlock();
		void				findExactMatchingLocationBlock();
		void				findLongestMatchingPrefixLocationBlock();
		bool				locationHasRedirection();
		bool				isMethodAllowed();

		void				handleResponseByMethod();
		void				handleGET();
		// void				handlePOST();
		// void				handleDELETE();
		
		void				handleFileResource();
		void				handleDirectoryResource();
		bool				directoryContainsIndexFile();
		void				setRequestedResource();
		std::string			getRequestedResourceType();

		bool				decodeURI();
		
		bool				getResponseIsReady();
		std::string&		getRawResponse();

		void				setRequest(Request* _Request);
		void				setResponsibleConfig();
		void				setvServerConfigs(const std::vector<ServerConfig>&			_vServerConfigs);
};

#endif