#pragma once

#include <string>
#include <regex>

#include "./database.h"
#include <bcrypt/BCrypt.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class authorization_service {
private:
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

        std::string username = data["username"];
        if (!is_valid_username(username))
            throw std::runtime_error("Incorrect username!");
        std::string email = data["email"];
        if (!is_valid_email(email))
            throw std::runtime_error("Incorrect email!");
        std::string password_hash = BCrypt::generateHash(data["password"]);

        auto transaction = db.get_transaction();
        try {
            transaction.exec(std::string("INSERT INTO ") + user_table_name +  "(username, email, password_hash) VALUES($1, $2, $3);", 
                pqxx::params(username, email, password_hash));
            transaction.commit();
        }
        catch (const std::exception& e) {
            transaction.abort();
            throw std::runtime_error("Incorrect data!");
        }
    }
};