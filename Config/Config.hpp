/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 13:01:00 by nazouz            #+#    #+#             */
/*   Updated: 2024/11/25 20:02:44 by nazouz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <fcntl.h>
#include <unistd.h>
// #include <pair>
#include <fstream>

#define RESET      "\033[0m"
#define BOLD       "\033[1m"
#define UNDERLINE  "\033[4m"
#define ITALIC     "\033[3m"
#define BLACK      "\033[30m"
#define RED        "\033[31m"
#define GREEN      "\033[32m"
#define YELLOW     "\033[33m"
#define BLUE       "\033[34m"
#define MAGENTA    "\033[35m"
#define CYAN       "\033[36m"
#define WHITE      "\033[37m"
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

typedef struct												LocationConfig {
	std::string												location;
	std::string 											root;
	std::vector<std::string> 								index;
	std::vector<std::string> 								methods;
	std::string 											upload_store;
	std::vector<std::string> 								redirect;
	std::string												autoindex;
	std::string 											cgi_pass;
	std::vector<std::string>								error_page;
	std::string												client_max_body_size;
}															LocationConfig;

typedef struct												ServerConfig {
	int														port;
	std::string												host;
	std::vector<std::string>								server_name;
	std::map<std::string, LocationConfig>					locations;
}															ServerConfig;

typedef struct												ServerConfigParser {
	std::map<std::string, std::string>						serverDirectives;
	std::vector< std::map<std::string, std::string> >		locationDirectives;
}															ServerConfigParser;

class Config {
	private:
		std::ifstream						configFile;
		std::vector<std::string>			configFileVector;
		
		std::vector< std::pair<int, int> >	serverBlocksIndexes;
		std::vector< std::pair<int, int> >	locationBlocksIndexes;
		
		std::map<std::string, std::string>	defaultServerDirectives;
		std::map<std::string, std::string>	defaultLocationDirectives;


		int									errorLog;

		std::vector<ServerConfig>			Servers;
		std::vector<ServerConfigParser>		Parser;
		
	public:
		Config();
		Config(const std::string& configFileName);
		~Config();

		bool						openConfigFile(const std::string& configFileName);
		void						fillDefaultDirectives();
		bool						parseConfigFile();
		bool						storeConfigFileInVector();
		
		bool						basicBlocksCountCheck();
		void						fillServerBlocksIndexes();
		std::pair<int, int>			getBlockEndIndex(int blockStart, const std::string startBlockStr);
		bool						validateBlocksIndexes();
		bool						locationBlockIsInsideAServerBlock(int locationStart, int locationEnd);
		
		bool						parseAllServerBlocks();
		bool						parseSingleServerBlock(int start, int end, ServerConfigParser& currentServer);
		bool						addToServerParserServerDirectives(const std::string& key, const std::string& value, std::map<std::string, std::string>& directives);
		bool						validateServerBlockDirectives(std::map<std::string, std::string>& directives);
		bool						parseSingleLocationBlock(int start, int end, ServerConfigParser& currentServer);
		bool						addToServerParserLocationDirectives(const std::string& key, const std::string& value, std::map<std::string, std::string>& directives);
		bool						validateLocationBlockDirectives(std::map<std::string, std::string>& directives);
		bool						isAllowedDirective(const std::string& directive, const std::string& blockType);
		void						Logger(std::string error);

		bool						constructServers();
		bool						fillServerConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, ServerConfig& Server);
		bool						fillLocationConfigByParserDirectives(std::map<std::string, std::string>& serverDirectives, std::map<std::string, std::string>& locationDirectives, ServerConfig& Server);

};

bool					isValidPort(const std::string& port);
bool					isValidHost(const std::string& host);
bool					isValidServerName(const std::string& serverName);
bool					isValidErrorPage(const std::string& errorPage);
bool					isValidClientMaxBodySize(std::string& client_max_body_size);
bool					isValidPath(const std::string& path);
bool					isValidIndex(const std::string& index);
bool					isValidMethods(const std::string& methods);
bool					isValidRedirect(const std::string& redirect);
bool					isValidAutoIndex(const std::string& autoindex);
bool					isValidCgiPass(const std::string& cgi_pass);
std::string				stringtrim(const std::string& str, const std::string& set);
bool					stringIsDigit(const std::string& str);

#endif