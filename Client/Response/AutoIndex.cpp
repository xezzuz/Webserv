#include "Response.hpp"
#include "Error.hpp"

void	Response::initDirList(std::string& list)
{
	dirList = opendir(input.path.c_str()); // CHECK LEAK
	if (dirList == NULL)
	{
		std::cerr << "[WEBSERV][ERROR]\t>";
		perror("opendir");
		throw(ErrorPage(500));
	}
	list = "<html>\n"
			"<head>\n"
			"<title>Index of " + input.uri + "</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1>Index of " + input.uri + "</h1>\n"
			"<hr>\n"
			"<pre>\n";
	
}

void	Response::directoryListing()
{
	std::string		list;
	struct dirent	*entry;
	int 			i = 0;

	if (!dirList)
		initDirList(list);

	while (i < 100 && (entry = readdir(dirList)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == ".")
			continue ;
		if (entry->d_type == DT_DIR)
			name.append("/");
		list.append("<a href=\"" + name + "\">" + name + "</a>\n");
		i++;
	}
	if (entry == NULL)
	{
		list.append("</pre>\n"
					"<hr>\n"
					"</body>\n"
					"</html>");
		closedir(dirList);
		dirList = NULL;
		nextState = FINISHED;
	}
	buffer = buildChunk(list.c_str(), list.size());
	state = SENDDATA;
}
