import os
import sys

def main():
    # Get content length from environment
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    
    # Read exactly content_length bytes from stdin as binary data
    input_data = bytearray()
    bytes_read = 0
    
    while bytes_read < content_length:
        chunk = sys.stdin.buffer.read(content_length - bytes_read)
        if not chunk:
            break  # EOF reached unexpectedly
        input_data.extend(chunk)
        bytes_read += len(chunk)
    
    # Check if there's extra data beyond content_length
    extra_data = sys.stdin.buffer.read(1)  # Try to read one more byte
    if extra_data:
        print("Status: 999 Debug\r")
        print("Content-Type: text/plain\r\n\r")
        print("Extra data detected beyond CONTENT_LENGTH.")
        return
    
    try:
        # Write the received data to a file as binary
        with open("data.png", "wb") as file:
            file.write(input_data)
        
        # Print the success HTTP status
        print("Status: 201 Created\r")
        print("Content-Type: text/plain\r\n\r")
        print("Data successfully written.")
    except Exception as e:
        # Print the failure HTTP status
        print("Status: 503 Service Unavailable\r")
        print("Content-Type: text/plain\r\n\r")
        print("Error writing to file: ", str(e))

if __name__ == "__main__":
    main()