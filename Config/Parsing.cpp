/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 20:20:43 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/24 20:22:09 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool				Config::openConfigFile(const std::string& configFileName) {
	int					pos = configFileName.find(".conf");
	if (pos == 0 || pos == std::string::npos || configFileName.substr(pos) != ".conf")
		return false;
	
	configFile.open(configFileName, std::ios::in);
	if (!configFile.is_open())
		return false;
	return true;
}

bool				Config::parseConfigFile() {
	storeConfigFileInVector();
	if (!basicBlocksCountCheck())
		return false;
	fillServerBlocksIndexes();
	if (!validateBlocksIndexes()) {
		std::cout << "[BLOCKS MISPLACEMENT]\n";
		return false;
	}
	if (!validateAllServerBlocks()) {
		std::cout << "[SERVER BLOCK]\n";
		return false;
	}
	return true;
}

bool				Config::storeConfigFileInVector() {
	std::string			line;

	while (std::getline(configFile, line)) {
		if (line.empty())
			continue;
		if (stringtrim(line, " \t").empty())
			continue;
		if (stringtrim(line, " \t")[0] == '#')
			continue;
		configFileVector.push_back(line);
		if (configFile.eof())
			break;
	}
	return true;
}

bool				Config::basicBlocksCountCheck() {
	int					serverStart = 0;
	int					serverEnd = 0;
	int					locationStart = 0;
	int					locationEnd = 0;
	
	for (size_t i = 0; i < configFileVector.size(); i++) {
		if (configFileVector[i] == "[server]")
			serverStart++;
		else if (configFileVector[i] == "[/server]")
			serverEnd++;
		else if (configFileVector[i] == "[location]")
			locationStart++;
		else if (configFileVector[i] == "[/location]")
			locationEnd++;
	}
	std::cout << "[server] = " << serverStart << " [/server] = " << serverEnd << std::endl;
	std::cout << "[location] = " << locationStart << " [/location] = " << locationEnd << std::endl;
	if (!serverStart || !serverEnd)
		return false;
	if ((serverStart != serverEnd) || (locationStart != locationEnd))
		return false;
	std::cout << "basicBlocksCountCheck(true)" << std::endl;
	return true;
}

std::pair<int, int>		Config::getBlockEndIndex(int blockStart, const std::string startBlockStr) {
	int						i = blockStart;
	std::string				endBlock;
	std::pair<int, int>		toReturn(blockStart, -1);
	
	if (startBlockStr == "[server]")
		endBlock = "[/server]";
	else if (startBlockStr == "[location]")
		endBlock = "[/location]";

	for (; i < configFileVector.size(); i++) {
		if (configFileVector[i] == endBlock) {
			toReturn = std::make_pair(blockStart, i);
			return toReturn;
		}
	}
	return toReturn;
}

void				Config::fillServerBlocksIndexes() {
	for (size_t i = 0; i < configFileVector.size(); i++) {
		if (configFileVector[i] == "[server]")
			serverBlocksIndexes.push_back(getBlockEndIndex(i, "[server]"));
		else if (configFileVector[i] == "[location]")
			locationBlocksIndexes.push_back(getBlockEndIndex(i, "[location]"));
	}
}

bool				Config::locationBlockIsInsideAServerBlock(int locationStart, int locationEnd) {
	// std::pair<int, int>		matchingServerBlock;
	for (size_t i = 0; i < serverBlocksIndexes.size(); i++) {
		if (serverBlocksIndexes[i].first < locationStart && serverBlocksIndexes[i].second > locationEnd)
			return true;
	}
	return false;
}

bool				Config::validateBlocksIndexes() {
	int			a, b, c, d;
	printf("SERVER BLOCKS:\n");
	for (size_t i = 0; i < serverBlocksIndexes.size() - 1; i++) {
		a = serverBlocksIndexes[i].first;
		b = serverBlocksIndexes[i].second;
		c = serverBlocksIndexes[i + 1].first;
		d = serverBlocksIndexes[i + 1].second;
		printf("[%d][%d] [%d][%d]\n", a, b, c, d);
		if (a == -1 || b == -1 || c == -1 || d == -1)
			return false;
		if (a < b && b < c && c < d)
			continue;
		return false;
	}
	if (!locationBlocksIndexes.size()) {
		std::cout << "validateBlocksIndexes(true)" << std::endl;
		return true;
	}
	printf("LOCATION BLOCKS:\n");
	for (size_t i = 0; i < static_cast<int>(locationBlocksIndexes.size() - 1); i++) {
		a = locationBlocksIndexes[i].first;
		b = locationBlocksIndexes[i].second;
		c = locationBlocksIndexes[i + 1].first;
		d = locationBlocksIndexes[i + 1].second;
		printf("[%d][%d] [%d][%d]\n", a, b, c, d);
		if (a == -1 || b == -1 || c == -1 || d == -1)
			return false;
		if (!(a < b && b < c && c < d))
			return false;
		if (locationBlockIsInsideAServerBlock(a, b))
			continue;
		return false;
	}
	a = locationBlocksIndexes[locationBlocksIndexes.size() - 1].first;
	b = locationBlocksIndexes[locationBlocksIndexes.size() - 1].second;
	if (!locationBlockIsInsideAServerBlock(a, b)) {
		printf("here\n");
		return false;
	}
	std::cout << "validateBlocksIndexes(true)" << std::endl;
	return true;
}