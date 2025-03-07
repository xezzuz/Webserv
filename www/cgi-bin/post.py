#!/usr/bin/env python3
import cgi
import os
import sys

def main():
    # Print HTTP headers
    print("Content-Type: text/plain\r")
    
    try:
        # Parse the form data
        form = cgi.FieldStorage()
        
        # Check if 'data' field is provided
        if 'data' not in form:
            print("Status: 400 Bad Request\r")
            print("\r\n\r")  # Empty line to separate headers from body
            print("Error: Missing 'data' parameter")
            return
        
        # Get data and filename
        data = form.getvalue('data')
        filename = form.getvalue('filename', 'output.txt')  # Default filename if not provided
        
        # Write data to file
        with open(filename, 'w') as f:
            f.write(data)
        
        # Return success
        print("Status: 200 OK\r")
        print("\r\n\r")  # Empty line to separate headers from body
        print(f"Success: Data written to {filename}")
        
    except Exception as e:
        # Return error
        print("Status: 500 Internal Server Error\r")
        print("\r\n\r")  # Empty line to separate headers from body
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    main()