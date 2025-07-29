#include "./application.h"

// invoker_t
std::shared_ptr<beast::http::response<beast::http::string_body>> invoker(
    std::shared_ptr<beast::http::request<beast::http::string_body>> request
) {
    std::cout << "Received: " << request->method() << " " << request->target() << std::endl;

    auto response = std::make_shared<beast::http::response<beast::http::string_body>>();
    response->version(request->version());
    response->result(beast::http::status::ok);
    response->set(beast::http::field::server, "Boost.Asio SSL Server");
    response->set(beast::http::field::content_type, "text/plain");
    response->body() = "Hello from SSL server!";
    response->prepare_payload();

    return response;
}

application::application()
    : server(io_context, 8443, invoker) {}

int application::execute() try {
    std::cout << "SSL server listening on port 8443..." << std::endl;
    io_context.run();
    
    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what();
    return 1;
}