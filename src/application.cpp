#include "./application.h"

// invoker_t
std::shared_ptr<beast::http::response<beast::http::string_body>> invoker(
    std::shared_ptr<beast::http::request<beast::http::string_body>> request
) {
    std::cout << "Received: " << request->method() << " " << request->target() << " " << request->body() << std::endl;

    auto response = std::make_shared<beast::http::response<beast::http::string_body>>();
    response->version(request->version());
    response->set(beast::http::field::server, "Boost.Asio SSL Server");
    response->set(beast::http::field::content_type, "text/plain");

    auto set_response = [&response](const std::string& body, beast::http::status result = beast::http::status::ok) {
        response->body() = body;
        response->result(result);
        response->prepare_payload();
    };

    if (request->target().starts_with("/api")) {
        set_response("Api isn\'t working yet.");
    }
    else if (request->target().starts_with("/ping")) {
        set_response("pong");
    }
    else {
        set_response("Incorrect request!", beast::http::status::bad_request);
    }
    
    // request[beast::http::field::content_type]

    return response;
}

application::application()
    : server(io_context, 8443, invoker), db("sBeHappy") {}

int application::execute() try {
    std::cout << "SSL server listening on port 8443..." << std::endl;
    io_context.run();
    
    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what();
    return 1;
}