#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

struct response_manager {
    using response_t = beast::http::response<beast::http::string_body>;
    using response_pointer_t = std::shared_ptr<response_t>;

    static auto make_response(unsigned int version, const std::string& server_name) {
        auto response = std::make_shared<response_t>();
        response->version(version);
        response->set(beast::http::field::server, server_name);
        return response;
    }

    static void edit_response(response_pointer_t response, const std::string& content_type, const std::string& body,
            beast::http::status result) {        
        response->set(beast::http::field::content_type, content_type);
        response->body() = body;
        response->result(result);
        response->prepare_payload();
    }
};
