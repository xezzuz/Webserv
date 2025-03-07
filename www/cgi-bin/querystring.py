#!/usr/bin/env python3
import cgi
import html

print("Content-Type: text/html\r\n\r\n", end="")

form = cgi.FieldStorage()
name = form.getvalue('name')

if name:
    print(f"<h1>Hello {name}!</h1>")
else:
    print("<h1>Hello stranger!</h1>")

print("</body></html>")