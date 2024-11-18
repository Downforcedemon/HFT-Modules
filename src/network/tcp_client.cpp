#include "/home/sm/CLionProjects/HFT_Modules/include/network/tcp_client.hpp"    // Our header file

#include <sys/socket.h>              // For socket functions
#include <netinet/in.h>              // For sockaddr_in
#include <arpa/inet.h>               // For inet_pton
#include <unistd.h>                  // For close()
#include <stdexcept>                 // For standard exceptions
#include <cerrno>                    // For errno
#include <cstring>                   // For strerror

namespace hft::network {

// Constructor
TcpClient::TcpClient()
    : socket_fd_(-1)                     // Initialize invalid socket
    , connected_(false)                  // Start disconnected
{
    // Nothing else to initialize
}

// Destructor
TcpClient::~TcpClient() {
    close_socket();                      // Clean up socket if open
}

// Move constructor
TcpClient::TcpClient(TcpClient&& other) noexcept
    : socket_fd_(other.socket_fd_)
    , connected_(other.connected_)
    , last_error_(std::move(other.last_error_))
{
    // Reset other object to default state
    other.socket_fd_ = -1;
    other.connected_ = false;
}

// Move assignment operator
TcpClient& TcpClient::operator=(TcpClient&& other) noexcept {
    if (this != &other) {           // Prevent self-assignment
        close_socket();             // Clean up existing socket

        // Move the resources
        socket_fd_ = other.socket_fd_;
        connected_ = other.connected_;
        last_error_ = std::move(other.last_error_);

        // Reset other object
        other.socket_fd_ = -1;
        other.connected_ = false;
    }
    return *this;
}

// Private helper method to close the socket
void TcpClient::close_socket() {
    if (socket_fd_ >= 0) {              // Check if socket is valid
        ::close(socket_fd_);            // Close the socket (:: means global namespace)
        socket_fd_ = -1;                // Reset to invalid state
        connected_ = false;             // Update connection state
        last_error_.clear();            // Clear any previous errors
    }
}

// Connection method
bool TcpClient::connect(const std::string& ip, uint16_t port) {
    // If already connected, close the existing connection
    if (connected_) {
        close_socket();
    }

    // Create new socket
    socket_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        set_last_error("Failed to create socket: " + std::string(std::strerror(errno)));
        return false;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));    // Zero out structure
    server_addr.sin_family = AF_INET;                     // IPv4
    server_addr.sin_port = htons(port);                   // Convert port to network byte order

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        set_last_error("Invalid address: " + ip);
        close_socket();
        return false;
    }

    // Attempt to connect to the server
    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        set_last_error("Failed to connect: " + std::string(std::strerror(errno)));
        close_socket();
        return false;
    }

    // Connection successful
    connected_ = true;
    last_error_.clear();  // Clear any previous errors
    return true;
}

// Disconnect from server
void TcpClient::disconnect() {
    if (connected_) {
        close_socket();
    }
}

// Check connection status
bool TcpClient::is_connected() const noexcept {
    return connected_;
}

// Get last error message
std::string TcpClient::get_last_error() const {
    return last_error_;
}

// Send data
bool TcpClient::send(const std::string& data) {
    // Check if we're connected
    if (!connected_ || socket_fd_ < 0) {
        set_last_error("Not connected");
        return false;
    }

    // Track how much data we've sent
    size_t total_sent = 0;
    size_t remaining = data.length();

    // Keep sending until all data is sent
    while (total_sent < data.length()) {
        ssize_t sent = ::send(socket_fd_,
                            data.c_str() + total_sent,
                            remaining,
                            0);  // No special flags

        if (sent < 0) {
            // Handle error based on errno
            if (errno == EINTR) {
                // Interrupted by signal, try again
                continue;
            }
            set_last_error("Send failed: " + std::string(std::strerror(errno)));
            return false;
        }

        total_sent += sent;
        remaining -= sent;
    }

    return true;
}

// Updated receive method
bool TcpClient::receive(std::string& data, size_t max_length) {
    // Check if we're connected
    if (!connected_ || socket_fd_ < 0) {
        set_last_error("Not connected");
        return false;
    }

    // Clear the output string
    data.clear();

    // Buffer for receiving data
    char buffer[1024];  // Use a reasonably sized buffer

    // Try to receive data
    ssize_t received = ::recv(socket_fd_,
                            buffer,
                            std::min(sizeof(buffer), max_length),
                            0);  // No special flags

    if (received < 0) {
        // Handle error based on errno
        if (errno == EINTR) {
            // Interrupted by signal
            set_last_error("Receive interrupted");
            return false;
        }
        set_last_error("Receive failed: " + std::string(std::strerror(errno)));
        return false;
    }

    if (received == 0) {
        // Connection closed by peer
        set_last_error("Connection closed by peer");
        close_socket();
        return false;
    }

    // Append received data to output string
    data.append(buffer, received);
    return true;
}

// Error handling helper
void TcpClient::set_last_error(const std::string& error) {
    last_error_ = error;    // Store the error message
}

} // namespace hft::network