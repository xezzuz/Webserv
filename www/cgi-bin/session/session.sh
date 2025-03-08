#!/bin/sh
echo -e "Content-type: text/html\r"

SERVER_NAME="${SERVER_HOST:-localhost}"
SERVER_PORT="${SERVER_PORT:-8080}"
SCHEME="http"

BASE_URL="${SCHEME}://${SERVER_NAME}:${SERVER_PORT}/"

if [ "$HTTP_COOKIE" ]; then

	COOKIE_STR="$HTTP_COOKIE"
    SESSION_PART=${COOKIE_STR##*session_id=}
    SESSION_ID=${SESSION_PART%%;*}

    SESSION_FILE="/tmp/$SESSION_ID"
    
    if [ -f "$SESSION_FILE" ]; then
        EXPIRY=$(cat "$SESSION_FILE")
        NOW=$(date +%s)
        
        if [ "$NOW" -gt "$EXPIRY" ]; then
            rm "$SESSION_FILE"
            echo -e "Location: ${BASE_URL}\r"
            echo -e "\r"
            exit 0
        fi
    else
        SESSION_ID=""
    fi
fi

if [ -z "$SESSION_ID" ]; then
    SESSION_ID=$(date +%s | md5sum | head -c 16)
    EXPIRY=$(($(date +%s) + 10))
    echo "$EXPIRY" > "/tmp/$SESSION_ID"
    echo -e "Set-Cookie: session_id=$SESSION_ID; Path=/\r"
fi

echo -e "\r"
echo "<html><body>"
echo "<h2>Shell Session Active</h2>"
echo "<p>Session: $SESSION_ID</p>"
echo "<p>Refresh to test. Expires in 10 seconds.</p>"
echo "</body></html>"