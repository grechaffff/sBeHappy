#pragma once

#include <iostream>

#include <core/tcp_server>
#include <core/database>
#include <core/authorization_service>

class application {
private:
    asio::io_context io_context;
    tcp_server server;

    pqxx_wrapper db; // pqxx_wrapper = database

    authorization_service auth_service;

    void invoker(
        std::shared_ptr<beast::http::request<beast::http::string_body>> request,
        std::shared_ptr<beast::http::response<beast::http::string_body>> response
    );

public:
    application(const std::string& postgres_setting, std::string user_table, tcp_server_config server_config);
    
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    
    int execute();
    void stop();
};