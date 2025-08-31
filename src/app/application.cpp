#include "./application.h"

#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include "../modules/authorization_service_module/jwt_manager.h"

// invoker_t
void application::no_route_invoker(
    request_pointer_t request,
    response_pointer_t response
) {
    std::string body = fmt::format("There are no route for target {} and method {}.",
        request->target(), request->method_string());
    spdlog::error(body);
    response_manager::edit_response(response, "text/plain", body, beast::http::status::bad_request);
}

application::application(
    const std::string& postgres_setting,
    config_t<authorization_service> auth_service_config,
    config_t<https_server> server_config
)   : io_context()
    , server(std::move(server_config), io_context,
        std::bind(&application::no_route_invoker, this, std::placeholders::_1, std::placeholders::_2))
    , db(postgres_setting)
    , auth_service(db, std::move(auth_service_config)) {}

int application::execute() try {
    server.set("/api/register", [this](request_pointer_t request, response_pointer_t response){
        if (!(request->method_string() == "POST" && (*request)[beast::http::field::content_type] == "application/json")) {
            response_manager::edit_response(response, "text/plain", "Incorrect request!", beast::http::status::bad_request);
            return;
        }
        
        std::string jwt_token;
        try {
            jwt_token = auth_service.register_(request->body());
        }
        catch (const std::exception& e) {
            response_manager::edit_response(response, "text/plain", e.what(), beast::http::status::bad_request);
            return;
        }

        response_manager::edit_response(response, "text/plain", jwt_token, beast::http::status::ok);
    });

    server.set("/api/login", [this](request_pointer_t request, response_pointer_t response){
        if (!(request->method_string() == "POST" && (*request)[beast::http::field::content_type] == "application/json")) {
            response_manager::edit_response(response, "text/plain", "Incorrect request!", beast::http::status::bad_request);
            return;
        }
        
        std::string jwt_token;
        try {
            jwt_token = auth_service.login(request->body());
        }
        catch (const std::exception& e) {
            response_manager::edit_response(response, "text/plain", e.what(), beast::http::status::bad_request);
            return;
        }

        response_manager::edit_response(response, "text/plain", jwt_token, beast::http::status::ok);
    });
    
    server.set("/api", [](request_pointer_t request, response_pointer_t response){
        response_manager::edit_response(response, "text/plain", 
            "use /api/register to register: method - POST, conted_type - application/json, body - \n"
            "{\n\t\"username\":<username>,\n\t\"email\":<email>,\n\t\"password\":<password>\n}\n",
            beast::http::status::ok
        );
    });

    server.set("/ping", [](request_pointer_t request, response_pointer_t response){
        response_manager::edit_response(response, "text/plain", "pong", beast::http::status::ok);
    });
    
    server.set("/api/jwt/create", [](request_pointer_t request, response_pointer_t response){
        response_manager::edit_response(
            response,
            "text/plain", 
            jwt_manager::create_token(request->body(), "SERVER", std::getenv("JWT_SECRET")),
            beast::http::status::ok
        );
    });
    
    server.set("/api/jwt/verify", [](request_pointer_t request, response_pointer_t response){
        response_manager::edit_response(
            response,
            "text/plain", 
            jwt_manager::verify_token(request->body(), "SERVER", std::getenv("JWT_SECRET")) ? "true" : "false",
            beast::http::status::ok
        );
    });

    spdlog::info("SSL server listening on port 8443...");
    io_context.run();
    spdlog::info("SSL server stoped.");
    
    return 0;
}
catch (const std::exception& e) {
    spdlog::critical("{}", e.what());
    return 1;
}

void application::stop() {
    io_context.stop();
}