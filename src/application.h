#pragma once

#include <iostream>

#include "./tcp_server.h"
#include "./database.h"

class application {
private:
    asio::io_context io_context;
    tcp_server server;

public:
    application();
    
    application(const application&) = delete;
    application& operator=(const application&) = delete;

    application(application&&) = default;
    application& operator=(application&&) = default;
    
    int execute();
};