/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/23 19:56:25 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
// #include <pair>
#include <fstream>

#define RED "\033[31m"
#define GREEN "\033[32m"

typedef struct								LocationConfig {
	
}											LocationConfig;

typedef struct								ServerConfig {
	int										isValid;
	int										port;
	std::string								host;
	std::string								server_name;
	std::vector<std::string>				serverNames;
	std::string								errorPage;
	size_t									clientMaxBodySize;
	std::map<std::string, LocationConfig>	locations;
}											ServerConfig;

class Config {
	private:
		std::ifstream						configFile;
		std::vector<std::string>			configFileVector;
		std::vector< std::pair<int, int> >	serverBlocksIndexes;
		std::vector< std::pair<int, int> >	locationBlocksIndexes;

		std::vector<ServerConfig>			servers;
		
	public:
		Config();
		Config(const std::string& configFileName);
		~Config();

		bool						openConfigFile(const std::string& configFileName);
		bool						parseConfigFile();
		bool						storeConfigFileInVector();
		bool						basicBlocksCountCheck();
		void						fillServerBlocksIndexes();
		std::pair<int, int>			getBlockEndIndex(int blockStart, const std::string startBlockStr);
		bool						validateBlocksIndexes();
		bool						locationBlockIsInsideAServerBlock(int locationStart, int locationEnd);
		bool						validateAllServerBlocks();
		bool						validateSingleServerBlock(int start, int end, ServerConfig& currentServer);

};

bool					isValidPort(const std::string& port);
bool					isValidHost(const std::string& host);
bool					isValidServerName(const std::string& serverName);
bool					isValidErrorPage(const std::string& errorPage);
std::string				stringtrim(const std::string& str, const std::string& set);

#endif