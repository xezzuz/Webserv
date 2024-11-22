/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ControlCenter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 10:39:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/21 18:52:30 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void			Request::feedRequest(char *recvBuffer, int recvBufferSize) {
	std::cout << "WRITE OPERATION OF SIZE [" << recvBufferSize << "]" << std::endl;
	write(bufferFD, recvBuffer, recvBufferSize);

	
	std::string		recvBuff(recvBuffer, recvBufferSize);
	// buffer += recvBuff.substr(0, recvBufferSize);
	buffer += recvBuff.substr(0, recvBufferSize);
	std::cout << "[REQUEST]\tRequest got Fed! OldBufferSize = " << this->bufferSize << "\n";
	bufferSize += recvBufferSize;
	std::cout << "[REQUEST]\tRequest got Fed! NewBufferSize = " << this->bufferSize << "\n";
	parseControlCenter();
	
}

// PARSING CONTROL CENTER
bool			Request::parseControlCenter() {
	std::cout << "parseControlCenter();\n";
	if (pState == PARSING_FINISHED) {
		std::cout << "PARSING FINISHED parseControlCenter(true);\n";
		return true;
	}
	parseRequestLineAndHeaders();
	parseRequestBody();
	// if (pState == PARSING_FINISHED)
	// 	putRequestBodyInFile();
	if (pState == BODY_DECODED)
		std::cout << "BODY DECODED" <<	std::endl;
	if (pState == PARSING_FINISHED)
		std::cout << "PARSING FINISHED" <<	std::endl;
	std::cout << "parseControlCenter(true);\n";
	// std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
	// std::cout << buffer << std::endl;
	// std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
	return true;
}
