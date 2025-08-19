#pragma once

#include <iostream>

#include "./tcp_server.h"
#include "./database.h"
#include "./authorization_service.h"

class application {
private:
    asio::io_context io_context;
    tcp_server server;

    pqxx_wrapper db; // pqxx_wrapper = database

    authorization_service auth_service;

    std::shared_ptr<beast::http::response<beast::http::string_body>> invoker(
        std::shared_ptr<beast::http::request<beast::http::string_body>> request
    );

public:
    application(const std::string& postgres_setting, std::string user_table, tcp_server_config server_config);
    
    application(const application&) = delete;
    application& operator=(const application&) = delete;
    
    int execute();
};