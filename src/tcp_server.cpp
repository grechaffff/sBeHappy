#include "./tcp_server.h"

tcp_server::tcp_server(asio::io_context& io_context, unsigned short port, std::function<invoker_t> invoker)
    : context_(ssl::context::tlsv13)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , invoker(std::move(invoker)) {
        
    // Configuring the SSL context
    context_.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);
        
    // Installing the certificate and private key
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", ssl::context::pem);
        
    // Start accepting connections
    do_accept();
}

void tcp_server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Create an SSL socket and perform a handshake
                auto ssl_socket = std::make_shared<ssl::stream<tcp::socket>>(
                    std::move(socket), context_);
                
                ssl_socket->async_handshake(
                    ssl::stream_base::server,
                    [this, ssl_socket](boost::system::error_code ec) {
                        if (!ec) {
                            // After a successful handshake, we process the connection
                            handle_connection(ssl_socket);
                        } else {
                            std::cerr << "Handshake failed: " << ec.message() << "\n";
                        }
                    });
            }
                
            // Accept the following connection
            do_accept();
        }
    );
}

void tcp_server::handle_connection(std::shared_ptr<ssl::stream<tcp::socket>> ssl_socket) {
    auto buffer = std::make_shared<beast::flat_buffer>();
    auto request = std::make_shared<beast::http::request<beast::http::string_body>>();
        
    // Reading the HTTP request
    beast::http::async_read(
        *ssl_socket,
        *buffer,
        *request,
        [this, ssl_socket, buffer, request](boost::system::error_code ec, size_t) {
            if (!ec) {
                auto response = invoker(request);
                if (!response)
                    return;

                // Sending a response
                beast::http::async_write(
                    *ssl_socket,
                    *response,
                    [ssl_socket, response](boost::system::error_code ec, size_t) {
                        // Closing the connection after sending the response
                        if (ec) {
                            std::cerr << "Write failed: " << ec.message() << "\n";
                        }
                    });
            }
        });
    }