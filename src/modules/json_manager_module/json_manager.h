#pragma once

#include <nlohmann/json.hpp>

struct json_manager {
    template <typename... Strings> 
    static bool contains(const nlohmann::json& json, Strings&&... keys) {
        return (json.contains(std::forward<Strings>(keys)) && ...);
    }
};
