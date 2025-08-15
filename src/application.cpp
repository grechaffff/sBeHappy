#include "./application.h"

#include <spdlog/spdlog.h>

// invoker_t
std::shared_ptr<beast::http::response<beast::http::string_body>> application::invoker(
    std::shared_ptr<beast::http::request<beast::http::string_body>> request
) {
    spdlog::info("Received: method - {}, target - {}, body - {}.\n", 
        boost::beast::http::to_string(request->method()), request->target(), request->body());

    auto response = std::make_shared<beast::http::response<beast::http::string_body>>();
    response->version(request->version());
    response->set(beast::http::field::server, "Boost.Asio SSL Server");
    response->set(beast::http::field::content_type, "text/plain");

    auto set_response = [&response](const std::string& body,
            beast::http::status result = beast::http::status::ok) -> decltype(response)& {
        response->body() = body;
        response->result(result);
        response->prepare_payload();
        return response;
    };

    if (request->target() == "/api/register") {
        if (!(request->method_string() == "POST" && (*request)[beast::http::field::content_type] == "application/json")) {
            return set_response("Incorrect request! use /api to get help", beast::http::status::bad_request);
        }
        
        try {
            auth_service.register_(request->body());
        }
        catch (const std::exception& e) {
            return set_response(std::string(e.what()) +  " use /api to get help");
        }

        return set_response("Successful registration!");
    }
    else if (request->target() == "/api") {
        set_response(
            "use /api/register to register: method - POST, conted_type - application/json, body - \n"
            "{\n\t\"username\":<username>,\n\t\"email\":<email>,\n\t\"password\":<password>\n}\n"
        );
    }
    else if (request->target() == "/ping") {
        set_response("pong");
    }
    else {
        set_response("Incorrect request!", beast::http::status::bad_request);
    }

    return response;
}

application::application()
    : io_context()
    , server(io_context, 8443, std::bind(&application::invoker, this, std::placeholders::_1))
    , db("sBeHappy", "postgres", "host=postgres user=postgres")
    , auth_service(db, "users") {}

int application::execute() try {
    spdlog::info("SSL server listening on port 8443...");
    io_context.run();
    
    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what();
    return 1;
}