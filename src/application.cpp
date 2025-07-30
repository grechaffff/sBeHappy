#include "./application.h"

#include <bcrypt/BCrypt.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// invoker_t
std::shared_ptr<beast::http::response<beast::http::string_body>> application::invoker(
    std::shared_ptr<beast::http::request<beast::http::string_body>> request
) {
    std::cout << "Received: " << request->method() << " " << request->target() << " " << request->body() << std::endl;

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
        if (!json::accept(request->body())) {
            return set_response("Incorrect json! use /api to get help", beast::http::status::bad_request);
        }

        auto data = json::parse(request->body());
        if (!(data.contains("username") && data.contains("email") && data.contains("password"))) {
            return set_response("Incorrect json! use /api to get help", beast::http::status::bad_request);
        }

        std::string username = data["username"];
        std::string email = data["email"];
        std::string password_hash = BCrypt::generateHash(data["password"]);

        try {
            db().exec("INSERT INTO users(username, email, password_hash) VALUES($1, $2, $3);", pqxx::params(username, email, password_hash));
            db.commit();
        }
        catch (...) {
            return set_response("Incorrect data!", beast::http::status::bad_request);
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
    : server(io_context, 8443, std::bind(&application::invoker, this, std::placeholders::_1)), db("sBeHappy") {}

int application::execute() try {
    std::cout << "SSL server listening on port 8443..." << std::endl;
    io_context.run();
    
    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what();
    return 1;
}