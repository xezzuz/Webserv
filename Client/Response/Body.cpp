#include "Response.hpp"
#include "Error.hpp"

void	Response::storeBody( void )
{
	uploadFile.open(std::tmpnam(NULL), std::ios::out | std::ios::trunc);
	if (!uploadFile.is_open())
		throw(500);

	uploadFile.write(buffer.c_str(), buffer.size());
	if (!uploadFile)
		throw(500);
	buffer.clear(); //
}

int	Response::receive( void )
{
	char	body[RECV_BUFFER_SIZE];
	ssize_t	bytesRead = recv(socket, body, RECV_BUFFER_SIZE, 0);
	if (bytesRead == -1)
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] recv: " + strerror(errno)));
	}
	else if (bytesRead > 0)
	{
		buffer.append(std::string(body, bytesRead));
		// process body
		storeBody();
	}
	else
	{
		throw(Disconnect("[CLIENT-" + _toString(socket) + "] CLOSED CONNECTION"));
	}
	return (0);
}

