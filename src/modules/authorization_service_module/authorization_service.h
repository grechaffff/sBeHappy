#pragma once

#include <string>
#include <regex>

#include <bcrypt/BCrypt.hpp>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <core/database>
#include <core/json_manager>
#include <core/utils>

class authorization_service;

template <>
struct config_t<authorization_service> {
    std::string user_table_name;
    std::string user_logs_table_name;
};

class authorization_service {
private:
    using json = nlohmann::json;
    
    database& db;
    config_t<authorization_service> config;

public:
    authorization_service(database& db, config_t<authorization_service>);

    std::string register_(std::string json_data);
    
    std::string login(std::string json_data);

    static bool is_valid_username(const std::string& name) {
        for (auto c : name) {
            // digits, uppercase, lowercase letters and '_'
            if (!(std::isalnum(c) || c == '_'))
                return false;
        }
        return name.size() ? std::isalpha(name[0]) : false;
    }
    
    static bool is_valid_email(const std::string& email) {
        const std::regex pattern(
            R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );
    
        return std::regex_match(email, pattern);
    }
};