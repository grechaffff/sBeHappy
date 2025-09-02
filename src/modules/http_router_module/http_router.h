#pragma once

#include <unordered_map>
#include <optional>

#include <core/boost_include>
#include <core/utils>

template <class http_server_t>
class http_router {
public:
    using response_t = http_server_t::response_t;
    using response_pointer_t = http_server_t::response_pointer_t;

    using request_t = http_server_t::request_t;
    using request_pointer_t = http_server_t::request_pointer_t;

    using invoker_t = typename http_server_t::invoker_t;
    using invoker_function_t = std::function<invoker_t>;

    http_router(config_t<http_server_t> config, asio::io_context& io_context, invoker_function_t no_route_invoker)
        : server(std::move(config), io_context, std::bind(&http_router::invoker, this, std::placeholders::_1, std::placeholders::_2))
        , no_route_invoker(std::move(no_route_invoker)) {}

    std::optional<invoker_function_t> set(const std::string target, invoker_function_t invoker) {
        auto [it, is_correct] = routes.insert({target, std::move(invoker)});
        if (is_correct)
            return it->second;
        else 
            return std::nullopt;
    }

    invoker_function_t get(const std::string target) {
        auto it = routes.find(target);
        return (it == routes.end()) ? no_route_invoker : it->second;
    }

private:
    http_server_t server;

    std::unordered_map<std::string, invoker_function_t> routes;
    invoker_function_t no_route_invoker;

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