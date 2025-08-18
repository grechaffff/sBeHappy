#pragma once

#include <string>
#include <regex>

#include "./database.h"

#include <bcrypt/BCrypt.hpp>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

class authorization_service {
private:
    using json = nlohmann::json;
    
    database& db;
    std::string user_table_name;

public:
    authorization_service(database& db, std::string user_table_name)
        : db(db), user_table_name(std::move(user_table_name)) {}

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

    void register_(std::string json_data) {
        if (!json::accept(json_data)) {
            throw std::runtime_error("Incorrect json!");
        }

        auto data = json::parse(json_data);
        if (!(data.contains("username") && data.contains("email") && data.contains("password"))) {
            throw std::runtime_error("Incorrect json!");
        }

        auto transaction = db.get_transaction();

        std::string username = data["username"];
        if (!is_valid_username(username))
            throw std::runtime_error("Incorrect username!");
        if (!transaction.exec(fmt::format("SELECT * FROM {} WHERE username = '{}';", user_table_name, username)).empty()) {
            transaction.abort();
            throw std::runtime_error("Username is already in use!");
        }

        std::string email = data["email"];
        if (!is_valid_email(email))
            throw std::runtime_error("Incorrect email!");
        if (!transaction.exec(fmt::format("SELECT * FROM {} WHERE email = '{}';", user_table_name, email)).empty()) {
            transaction.abort();
            throw std::runtime_error("Email is already in use!");
        }

        std::string password_hash = BCrypt::generateHash(data["password"]);

        try {
            transaction.exec(fmt::format("INSERT INTO {}(username, email, password_hash) VALUES(\'{}\', \'{}\', \'{}\');", 
                user_table_name, username, email, password_hash));
            transaction.commit();
        }
        catch (const std::exception& e) {
            transaction.abort();
            throw std::runtime_error("Incorrect data!");
        }
    }
};