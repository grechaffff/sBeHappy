#pragma once

#include <iostream>

#include "./tcp_server.h"
#include "./database.h"

class application {
private:
    asio::io_context io_context;
    tcp_server server;

    pqxx_wrapper db; // pqxx_wrapper = database

public:
    application();
    
    application(const application&) = delete;
    application& operator=(const application&) = delete;

    application(application&&) = default;
    application& operator=(application&&) = default;
    
    int execute();
};