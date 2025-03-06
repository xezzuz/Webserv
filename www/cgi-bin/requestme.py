#!/usr/bin/env python3
import requests
import sys

def fetch_url(url):
    try:
        response = requests.get(url)
        print(f"Status Code: {response.status_code}")
        print(response.text)
    except requests.exceptions.RequestException as e:
        print(f"Error: {e}", file=sys.stderr)

if __name__ == "__main__":
    while True:
        fetch_url("http://localhost:8080/cgi-bin/inf.py")


