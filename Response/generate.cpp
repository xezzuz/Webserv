#include "Response.hpp"

void	Response::generateErrorPage( void )
{

	 // if (error_page directive exists)
	 	// open the error page in bodyFD
	 //else
		data = "<!DOCTYPE html>"
				"<html>"
				"<head>"
				"    <title> " + _toString(input.status) + " " + statusCodes[input.status] + " </title>"
				"</head>"
				"<body>"
				"    <center>"
				"        <h1> " + _toString(input.status) + " " + statusCodes[input.status] + " </h1>"
				"    </center>"
				"    <hr>"
				"    <center>webserv/1.0</center>"
				"</body>"
				"</html>";
}

void	Response::handlePOST( void )
{
	data = "<!DOCTYPE html>"
			"<html lang=\"en\">"
			"<head>"
			"    <meta charset=\"UTF-8\">"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
			"    <title>Success Response</title>"
			"    <style>"
			"        body {"
			"            font-family: Arial, sans-serif;"
			"            margin: 40px auto;"
			"            max-width: 650px;"
			"            padding: 20px;"
			"        }"
			"        h1 {"
			"            color: #2c662d;"
			"        }"
			"        p {"
			"            color: #333;"
			"            line-height: 1.4;"
			"        }"
			"    </style>"
			"</head>"
			"<body>"
			"    <h1>Success</h1>"
			"    <p>Your request has been processed successfully.</p>"
			"    <p>The data has been saved to our system.</p>"
			"</body>"
			"</html>";
}
