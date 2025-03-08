#!/usr/bin/env python3

import os
import sys

UPLOAD_DIR = "/home/mmaila/goinfre/"
if not os.path.exists(UPLOAD_DIR):
	os.makedirs(UPLOAD_DIR)

print("Content-Type: text/plain\r\n\r\n", end="")

try:
	with open(os.path.join(UPLOAD_DIR, "uploaded_file"), "wb") as f:
		content_length = int(os.environ.get('CONTENT_LENGTH', 0))
		post_data = sys.stdin.buffer.read(content_length)
		f.write(post_data)
	print("File uploaded successfully!")
except Exception as e:
	print(f"Error: {e}")