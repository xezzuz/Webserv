#include <iostream>
#include <vector>
#include <cstring>      // For memset
#include <cstdlib>      // For exit
#include <arpa/inet.h>  // For sockaddr_in, inet_pton
#include <unistd.h>     // For close, read

int main() {
    const char* address = "127.0.0.1"; // Target address
    const int port = 8080;             // Target port
    const int num_clients = 100;       // Number of clients

    std::vector<int> sockets;

    for (int i = 0; i < num_clients; ++i) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket for client " << i << "\n";
            continue;
        }

        sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        if (inet_pton(AF_INET, address, &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid address for client " << i << "\n";
            close(sockfd);
            continue;
        }

        if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Client " << i << " failed to connect\n";
            close(sockfd);
            continue;
        }

        std::cout << "Client " << i << " connected to " << address << ":" << port << "\n";
        sockets.push_back(sockfd);
    }

    std::cout << "All clients connected. Waiting for disconnection...\n";

    // Keep checking if sockets are still connected
    while (!sockets.empty()) {
        for (auto it = sockets.begin(); it != sockets.end();) {
            char buffer;
            ssize_t bytes = recv(*it, &buffer, 1, MSG_PEEK);
            
            if (bytes == 0 || bytes == -1) { // Server closed connection or error
                std::cout << "Client disconnected\n";
                close(*it);
                it = sockets.erase(it);
            } else {
                ++it;
            }
        }

        usleep(500000); // Sleep for 500ms to avoid busy looping
    }

    std::cout << "All clients disconnected. Exiting...\n";
    return 0;
}
