#include "./application.h"

#include <spdlog/spdlog.h>

// invoker_t
void application::invoker(
    std::shared_ptr<beast::http::request<beast::http::string_body>> request,
    std::shared_ptr<beast::http::response<beast::http::string_body>> response
) {
    auto edit_response = [&response](const std::string& message, const std::string& log, beast::http::status result = beast::http::status::ok) {
        nlohmann::json body = {
            {"message", message},
            {"log", log}
        };
        response_manager::edit_response(response, "application/json", body.dump(), result);
    };

    if (request->target() == "/api/register") {
        if (!(request->method_string() == "POST" && (*request)[beast::http::field::content_type] == "application/json")) {
            edit_response("Incorrect request!", "use /api to get help", beast::http::status::bad_request);
            return;
        }
        
        try {
            auth_service.register_(request->body());
        }
        catch (const std::exception& e) {
            edit_response(e.what(),  "use /api to get help");
            return;
        }

        edit_response("Successful registration!", "");
    }
    else if (request->target() == "/api") {
        edit_response(
            "",
            "use /api/register to register: method - POST, conted_type - application/json, body - \n"
            "{\n\t\"username\":<username>,\n\t\"email\":<email>,\n\t\"password\":<password>\n}\n"
        );
    }
    else if (request->target() == "/ping") {
        edit_response("", "pong");
    }
    else {
        edit_response("", "Incorrect request!", beast::http::status::bad_request);
    }
}

application::application(const std::string& postgres_setting, std::string user_table, tcp_server_config server_config)
    : io_context()
    , server(std::move(server_config), io_context,
        std::bind(&application::invoker, this, std::placeholders::_1, std::placeholders::_2))
    , db(postgres_setting)
    , auth_service(db, std::move(user_table)) {}

int application::execute() try {
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