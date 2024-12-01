/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 17:54:07 by nazouz            #+#    #+#             */
/*   Updated: 2024/12/01 20:38:55 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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
		
		Request*							_Request;
		ServerConfig						_Config;
		std::vector<ServerConfig>			vServerConfigs;

		t_statusline						statusLine;

		std::string							rawResponse;
		
	public:
		Response();
		~Response();

		void				feedResponse(Request* _Request);
		bool				generateResponse();
		
		bool				getResponseIsReady();
		std::string&		getRawResponse();

		void				setRequest(Request* _Request);
		void				setResponsibleConfig();
		void				setvServerConfigs(const std::vector<ServerConfig>&			_vServerConfigs);
};

#endif