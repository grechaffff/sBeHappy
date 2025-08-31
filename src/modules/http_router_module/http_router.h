#pragma once

#include <core/boost_include>

template <class T>
struct config_t;

template <class http_server_t>
class http_router {
public:
    using response_t = http_server_t::response_t;
    using response_pointer_t = http_server_t::response_pointer_t;

    using request_t = http_server_t::request_t;
    using request_pointer_t = http_server_t::request_pointer_t;

    using invoker_t = typename http_server_t::invoker_t;

    http_router(config_t<http_server_t> config, asio::io_context& io_context, std::function<invoker_t> no_route_invoker)
        : server(std::move(config), io_context, std::bind(&http_router::invoker, this, std::placeholders::_1, std::placeholders::_2))
        , no_route_invoker(std::move(no_route_invoker)) {}

    void set(const std::string target, std::function<invoker_t> invoker) {
        routes.insert({target, std::move(invoker)});
    }

    std::function<invoker_t> get(const std::string target) {
        auto it = routes.find(target);
        return (it == routes.end()) ? no_route_invoker : it->second;
    }

private:
    http_server_t server;

    std::unordered_map<std::string, std::function<invoker_t>> routes;
    std::function<invoker_t> no_route_invoker;

    // invoker_t
    void invoker(request_pointer_t request, response_pointer_t response) {
        for (auto& [target, invoker] : routes) {
            if (target == request->target()) {
                invoker(request, response);
                return; // success
            }
        }
        // not success
        no_route_invoker(request, response);
    }

};