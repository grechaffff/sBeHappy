#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <memory>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

class tcp_server {
public:
    using invoker_t = std::shared_ptr<beast::http::response<beast::http::string_body>>(
        std::shared_ptr<beast::http::request<beast::http::string_body>>
    );

    // invoker will be called when a request is received from the client
    tcp_server(asio::io_context& io_context, unsigned short port, std::function<invoker_t> invoker);

private:
    ssl::context context_;
    tcp::acceptor acceptor_;
    std::function<invoker_t> invoker;

    void do_accept();

    void handle_connection(std::shared_ptr<ssl::stream<tcp::socket>> ssl_socket);
};