#include "../../include/network/tcp_client.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

int main() {
    hft::network::TcpClient client;

    std::cout << "Attempting to connect to localhost:12345...\n";
    if (!client.connect("127.0.0.1", 12345)) {
        std::cerr << "Connection failed: " << client.get_last_error() << std::endl;
        return 1;
    }
    std::cout << "Connected successfully!\n";

    const std::vector<std::string> test_messages = {
        "Hello Server!",
        "Test Message 1",
        "Test Message 2",
        "QUIT"
    };

    for (const auto& msg : test_messages) {
        std::cout << "\nSending: " << msg << std::endl;

        if (!client.send(msg)) {
            std::cerr << "Failed to send: " << client.get_last_error() << std::endl;
            break;
        }
        std::cout << "Send successful\n";

        // Wait for response
        std::string response;
        std::cout << "Waiting for response...\n";
        if (!client.receive(response, 1024)) {
            std::cerr << "Failed to receive: " << client.get_last_error() << std::endl;
            break;
        }

        std::cout << "Received: " << response << std::endl;

        std::cout << "Waiting 500ms before next message...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\nAll messages sent. Disconnecting...\n";
    client.disconnect();
    std::cout << "Disconnected from server\n";

    return 0;
}