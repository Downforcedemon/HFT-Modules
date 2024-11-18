#ifndef HFT_MODULES_TCP_CLIENT_HPP
#define HFT_MODULES_TCP_CLIENT_HPP

#include <string>
#include <system_error>
#include <memory>


    namespace hft::network {

        class TcpClient {
        public:
            // Default constructor and destructor
            TcpClient();
            ~TcpClient();

            // Prevent copying
            TcpClient(const TcpClient&) = delete;
            TcpClient& operator=(const TcpClient&) = delete;

            // Allow moving
            TcpClient(TcpClient&&) noexcept;
            TcpClient& operator=(TcpClient&&) noexcept;

            // Connection methods
            bool connect(const std::string& ip, uint16_t port);
            void disconnect();

            // Status checking
            [[nodiscard]] bool is_connected() const noexcept;

            // Send/Receive methods
            bool send(const std::string& data);
            bool receive(std::string& data, size_t max_length);

            // Error handling
            [[nodiscard]] std::string get_last_error() const;

        private:
            // Private member variables will go here
            int socket_fd_{-1};                  // Socket file descriptor
            bool connected_{false};              // Connection state
            std::string last_error_;             // Last error message

            // Add any helper methods here
            void close_socket();                 // Helper to close socket
            void set_last_error(const std::string& error);  // Error setting helper
        };

    } // namespace hft::network


#endif // HFT_MODULES_TCP_CLIENT_HPP