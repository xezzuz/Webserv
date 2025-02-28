#!/usr/bin/env python3

import http.cookies
import os
import uuid
import json
import time

SESSION_DIR = "/tmp/cgi_sessions"
os.makedirs(SESSION_DIR, exist_ok=True)

# Set up cookies
cookie = http.cookies.SimpleCookie()
cookie_header = os.environ.get("HTTP_COOKIE")
session_id = None
session_count = 1
session_expiration = None  # Will be determined based on new/existing session

print("Content-Type: text/html", end="\r\n")

# Load existing session or create new
if cookie_header:
    cookie.load(cookie_header)
    if "session_id" in cookie:
        session_id = cookie["session_id"].value
        session_file = os.path.join(SESSION_DIR, session_id)
        if os.path.exists(session_file):
            try:
                with open(session_file, "r") as f:
                    session_data = json.load(f)
                # Preserve original expiration time
                session_expiration = session_data["expires"]
                session_count = session_data.get("count", 0) + 1
            except (json.JSONDecodeError, KeyError):
                # Handle corrupted file by creating new session
                session_id = None

# Create new session if needed
if not session_id or not session_expiration:
    session_id = str(uuid.uuid4())
    session_count = 1
    # Set expiration to 30 seconds from now for new sessions
    session_expiration = int(time.time()) + 30

# Update cookies with PROPERLY FORMATTED expiration time
cookie["session_id"] = session_id
cookie["session_id"]["path"] = "/"
cookie["session_id"]["expires"] = time.strftime(
    "%a, %d-%b-%Y %H:%M:%S GMT",
    time.gmtime(session_expiration)
)

# Save session data (preserve original expiration time)
session_file = os.path.join(SESSION_DIR, session_id)
with open(session_file, "w") as f:
    json.dump({
        "count": session_count,
        "expires": session_expiration  # Keep original expiration
    }, f)

# Print headers
print(cookie, end="\r\n")
print("", end="\r\n")

# HTML Response
print(f"""
<!DOCTYPE html>
<html>
<head>
    <title>Python CGI Session Example</title>
</head>
<body>
    <h2>Session Tracking with Cookies</h2>
    <p><strong>Session ID:</strong> {session_id} </p>
    <p><strong>Session Visit Count:</strong> {session_count} </p>
    <p><strong>Expires at:</strong> {time.ctime(session_expiration)}</p>
    <p><a href='/cgi-bin/session.py'>Refresh Page</a> to increment session count.</p>
</body>
</html>
""")