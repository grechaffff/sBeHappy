#pragma once

#include <string>

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

    void register_(std::string json_data) {
        if (!json::accept(json_data)) {
            throw std::runtime_error("Incorrect json!");
        }

        auto data = json::parse(json_data);
        if (!(data.contains("username") && data.contains("email") && data.contains("password"))) {
            throw std::runtime_error("Incorrect json!");
        }

        std::string username = data["username"];
        std::string email = data["email"];
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