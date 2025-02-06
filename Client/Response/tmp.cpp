#include "Response.hpp"
#include "Error.hpp"

// bool	Response::parseRangeHeader( void ) // example => Range: bytes=0-499,1000-1499
// {
// 	std::string	value = input.requestHeaders["Range"];
// 	std::string prefix = "bytes=";
// 	size_t pos = value.find(prefix);
// 	if (pos == std::string::npos)
// 		return (false);
// 	pos += prefix.length();


// 	std::istringstream	rangess(value.substr(pos));
// 	std::string			rangeStr;

// 	boundary = generateRandomString();

// 	while (std::getline(rangess, rangeStr, ','))
// 	{
// 		unsigned long start;
// 		unsigned long end;
// 		std::string startStr;
// 		std::string endStr;
// 		size_t	delim;
// 		char	*stop;

// 		stringtrim(rangeStr, " \t");
// 		delim = rangeStr.find("-");
// 		if (delim == std::string::npos)
// 			return (false);

// 		startStr = rangeStr.substr(0, delim);
// 		endStr = rangeStr.substr(delim + 1);
// 		if (startStr.empty() && endStr.empty())
// 			return (false);

// 		if (!startStr.empty())
// 		{
// 			start = std::strtoul(startStr.c_str(), &stop, 10); // 10: base decimal
// 			if (errno == ERANGE || *stop)
// 				return (false);
// 			if (endStr.empty())
// 				end = contentLength - 1;
// 		}
// 		if (!endStr.empty())
// 		{
// 			end = std::strtoul(endStr.c_str(), &stop, 10); // 10: base decimal
// 			if (errno == ERANGE || *stop)
// 				return (false);
// 			if (startStr.empty())
// 			{
// 				start = contentLength - end;
// 				end = contentLength - 1;
// 			}
// 		}

// 		if (start > end || end >= contentLength)
// 			throw(ErrorPage(416));

// 		std::cout << "Start >> " << start << " | End >> " << end << std::endl;
// 		Range unit;

// 		unit.range = std::make_pair(start, end);
// 		unit.rangeLength = end - start + 1;
	
// 		unit.header.append("\r\n--" + boundary);
// 		unit.header.append("\r\nContent-Range: bytes " + startStr + "-" + endStr + "/" + _toString(contentLength));
// 		unit.header.append("\r\nContent-Type: " + contentType);
// 		unit.header.append("\r\n\r\n");

// 		ranges.push_back(unit);
// 	}
// 	ranges[0].header.erase(0, 2); // erases the first "\r\n"
// 	return (true);
// }

// int	Response::rangeContentLength( void )
// {
// 	int length = 0;
// 	std::vector<Range>::iterator it = ranges.begin();

// 	for (; it != ranges.end(); it++)
// 	{
// 		length += it->header.length();
// 		length += it->rangeLength;
// 	}
// 	length += 8 + boundary.length(); // 8 is  the length of the constant "\r\n--" "--\r\n"
// 	return (length);
// }

// bool	Response::buildRange( void )
// {
// 	if (input.requestHeaders.find("range") == input.requestHeaders.end())
// 		return (false);

// 	if (!parseRangeHeader())
// 		return (false);
	
// 	input.status = 206;
// 	if (ranges.size() == 1)
// 	{
// 		ranges[0].headerSent = true;
// 		contentLength = ranges[0].rangeLength;
// 		headers.append("\r\nContent-Range: bytes " + _toString(ranges[0].range.first) + "-" + _toString(ranges[0].range.second) + "/" + _toString(fileLength(input.path)));
// 	}
// 	else
// 	{
// 		contentType = "multipart/byteranges; boundary=" + boundary;
// 		contentLength = rangeContentLength();
// 	}
// 	state = NEXTRANGE;
// 	return (true);
// }

// void	Response::getNextRange()
// {
// 	if (currRange == ranges.size())
// 	{
// 		if (ranges.size() > 1)
// 		{
// 			buffer = "\r\n--" + boundary + "--\r\n";
// 			state = SENDDATA;
// 			nextState = FINISHED;
// 		}
// 		if (ranges.size() == 1)
// 		{
// 			state = FINISHED;
// 		}
// 	}
// 	else
// 	{
// 		std::cout << "RANGE LENGTH OF INDEX " << currRange << ": " << ranges[currRange].rangeLength << std::endl;
// 		if (ranges.size() > 1)
// 			buffer.append(ranges[currRange].header);
// 		bodyFile.seekg(ranges[currRange].range.first, std::ios::beg);
// 		state = READRANGE;
// 		nextState = READRANGE;
// 	}
// }

// void	Response::readRange()
// {
// 	char buf[SEND_BUFFER_SIZE] = {0};

// 	size_t readLength = std::min
// 	(
// 		static_cast<size_t>(SEND_BUFFER_SIZE),
// 		ranges[currRange].rangeLength
// 	);
// 	std::cout << "READ LENGTH OF RANGE :" << readLength << std::endl;
// 	int bytesRead = bodyFile.read(buf, readLength).gcount();
// 	if (bytesRead == -1)
// 	{
// 		throw(FatalError(strerror(errno)));
// 	}
// 	else if (bytesRead > 0)
// 	{
// 		buffer.append(std::string(buf, bytesRead));
// 		// std::cout << "BUFFER SIZE IN RANGE : " << buffer.size() << std::endl;
// 		// printState(state, "STATE");
// 		// printState(nextState, "NEXT_STATE");
// 		ranges[currRange].rangeLength -= bytesRead;
// 		state = SENDDATA;
// 		if (ranges[currRange].rangeLength == 0)
// 		{
// 			nextState = NEXTRANGE;
// 			currRange++;
// 		}
// 	}
// }
