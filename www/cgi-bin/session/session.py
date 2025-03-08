#!/usr/bin/env python3
import os, time, hashlib, http.cookies

print("Content-type: text/html", end="\r\n")

server_name = os.environ.get("SERVER_HOST", "localhost")
server_port = os.environ.get("SERVER_PORT", "8080")

scheme = "http"
base_url = f"{scheme}://{server_name}:{server_port}/"

cookies = http.cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookies.load(os.environ['HTTP_COOKIE'])

session_id = None
redirect = False

if 'session_id' in cookies:
    session_id = cookies['session_id'].value
    session_file = f"/tmp/{session_id}"
    
    if os.path.exists(session_file):
        with open(session_file, 'r') as f:
            expiry = int(f.read().strip())
            
        if time.time() > expiry:
            os.remove(session_file)
            redirect = True
    else:
        session_id = None

if session_id is None and not redirect:
    session_id = hashlib.md5(str(time.time()).encode()).hexdigest()[:16]
    expiry = int(time.time()) + 10
    
    with open(f"/tmp/{session_id}", 'w') as f:
        f.write(str(expiry))
    
    cookies['session_id'] = session_id
    cookies['session_id']['path'] = '/'
    print(cookies.output(), end="\r\n")

if redirect:
    print(f"Location: {base_url}", end="\r\n\r\n")
else:
    print("", end="\r\n\r\n")
    print(f"""<html><body>
<h2>Python Session Active</h2>
<p>Session: {session_id}</p>
<p>Refresh to test. Expires in 10 seconds.</p>
</body></html>""")