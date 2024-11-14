// Message Format (Request / Response)
/*
		HTTP-message   = start-line
		                      *( header-field CRLF )
		                      CRLF
		                      [ message-body ]
*/


/*********************** Request ***********************/

// Request Line
/*
		request-line   = method SP request-target SP HTTP-version CRLF
*/


// Header field
/*
		header-field   = field-name ":" OWS field-value OWS			// SP | HT
*/

// Body
/*
		The presence of a message body in a request is signaled by a
		Content-Length or Transfer-Encoding header field.
		Request message framing is independent of method semantics.

*/