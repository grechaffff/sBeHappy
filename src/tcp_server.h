#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

struct tcp_server_config {
    unsigned short port;
    std::string certificate_chain_file;
    std::string private_key_file;
    std::string server_name;

    unsigned int max_request_body_size = 256;
};

class tcp_server {
public:
    using invoker_t = void(
        std::shared_ptr<beast::http::request<beast::http::string_body>>,
        std::shared_ptr<beast::http::response<beast::http::string_body>>
    );

    // invoker will be called when a correct request is received from the client
    tcp_server(tcp_server_config config, asio::io_context& io_context, std::function<invoker_t> invoker);

private:
    tcp_server_config config;
    ssl::context context_;
    tcp::acceptor acceptor_;
    std::function<invoker_t> invoker;

    void do_accept();

    void handle_connection(std::shared_ptr<ssl::stream<tcp::socket>> ssl_socket);
};