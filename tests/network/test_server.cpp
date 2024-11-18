#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

bool running = true;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\nShutting down server...\n";
        running = false;
    }
}

int main() {
    signal(SIGINT, signal_handler);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options\n";
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket\n";
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        std::cerr << "Failed to listen\n";
        return 1;
    }

    std::cout << "Test server listening on port 12345...\n";
    std::cout << "Press Ctrl+C to shutdown\n";

    while (running) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (running) {
                std::cerr << "Failed to accept connection\n";
            }
            continue;
        }

        std::cout << "Client connected\n";

        char buffer[1024];
        while (running) {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_read < 0) {
                std::cerr << "Error reading from client\n";
                break;
            }
            if (bytes_read == 0) {
                std::cout << "Client disconnected\n";
                break;
            }

            buffer[bytes_read] = '\0';
            std::cout << "Received (" << bytes_read << " bytes): " << buffer << std::endl;

            // Echo back with prefix
            std::string response = "Server Echo: " + std::string(buffer);
            ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
            if (bytes_sent < 0) {
                std::cerr << "Error sending response\n";
                break;
            }
            std::cout << "Sent response (" << bytes_sent << " bytes)\n";
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}