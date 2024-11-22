/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/22 18:17:58 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void			Request::setStatusCode(int code) {
	statusCode = code;
	pState = PARSING_FINISHED;
}

void			Request::setRequestState() {
	// if we haven't received completed headers
	if (pState == PARSING_INIT && bufferContainHeaders())
		pState = HEADERS_RECEIVED;
	else if (pState == HEADERS_FINISHED && bufferSize != 0)
		pState = BODY_RECEIVED;
	else if (pState == BODY_FINISHED)
		pState = PARSING_FINISHED;

	if (pState == HEADERS_FINISHED && requestLine.method == "GET")
		pState = PARSING_FINISHED;
}

void			Request::feedRequest(char *recvBuffer, int recvBufferSize) {
	std::string		recvBuff(recvBuffer, recvBufferSize);
	buffer.append(recvBuffer, recvBufferSize);
	bufferSize += recvBufferSize;
	parseControlCenter();
}

// PARSING CONTROL CENTER
bool			Request::parseControlCenter() {
	switch (pState) {
		case HEADERS_RECEIVED:
			parseRequestLineAndHeaders();
			break;
		case BODY_RECEIVED:
			parseRequestBody();
			break;
		case PARSING_FINISHED:
			break;
		default:
			break;
	}
	return true;
}
