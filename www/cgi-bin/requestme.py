import requests
import sys

def fetch_url(url):
	try:
		response = requests.get(url)
		response.raise_for_status()  # Raise an error for bad responses (4xx and 5xx)
		print(response.text)
	except requests.exceptions.RequestException as e:
		print(f"Error: {e}", file=sys.stderr)

if __name__ == "__main__":
		fetch_url("http://localhost:8080/cgi-bin/requestme.py")

