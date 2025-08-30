#include "./authorization_service.h"

#include "./jwt_manager.h"

authorization_service::authorization_service(database& db, std::string user_table_name, std::string user_logs_table_name)
    : db(db), user_table_name(std::move(user_table_name)), user_logs_table_name(std::move(user_logs_table_name)) {}

std::string authorization_service::register_(std::string json_data) {
    auto [data, is_valid] = json_manager::create(json_data, "username", "email", "password");
    if (!is_valid) {
        throw std::runtime_error("Invalid json!");
    }

    auto transaction = db.get_transaction();

    std::string username = data["username"];
    if (!is_valid_username(username))
        throw std::runtime_error("Incorrect username!");
    if (!transaction.exec(fmt::format("SELECT * FROM {} WHERE username = $1;", user_table_name), pqxx::params(username)).empty()) {
        transaction.abort();
        throw std::runtime_error("Username is already in use!");
    }

    std::string email = data["email"];
    if (!is_valid_email(email))
        throw std::runtime_error("Incorrect email!");
    if (!transaction.exec(fmt::format("SELECT * FROM {} WHERE email = $1;", user_table_name), pqxx::params(email)).empty()) {
        transaction.abort();
        throw std::runtime_error("Email is already in use!");
    }

    std::string password_hash = BCrypt::generateHash(data["password"]);

    try {
        transaction.exec(fmt::format("INSERT INTO {}(username, email, password_hash) VALUES($1, $2, $3);", 
            user_table_name), pqxx::params(username, email, password_hash));
    }
    catch (const std::exception& e) {
        transaction.abort();
        throw std::runtime_error("Incorrect data!");
    }

    try {
        constexpr auto sql = "INSERT INTO {}(user_id, log_event) VALUES((SELECT id FROM {} WHERE username=$1), $2);";
        auto log = "Have been registered and received JWT token.";
        transaction.exec(fmt::format(sql, user_logs_table_name, user_table_name), pqxx::params(username, log));
        transaction.commit();
    }
    catch (const std::exception& e) {
        transaction.abort();
        throw std::runtime_error("Unknown error!");
    }

    return jwt_manager::create_token(username, "SERVER", std::getenv("JWT_SECRET"));
}

std::string authorization_service::login(std::string json_data) {
    auto [data, is_valid] = json_manager::create(json_data, "username", "password");
    if (!is_valid) {
        throw std::runtime_error("Invalid json!");
    }

    auto transaction = db.get_transaction();

    std::string username = data["username"];
    if (!is_valid_username(username))
        throw std::runtime_error("Incorrect username!");

    if (transaction.exec(fmt::format("SELECT * FROM {} WHERE username = $1;", user_table_name), pqxx::params(username)).empty()) {
        transaction.abort();
        throw std::runtime_error("Username is not in use!");
    }

    std::string password_hash = transaction.exec(fmt::format("SELECT password_hash FROM {} WHERE username = $1;", user_table_name), pqxx::params(username))[0][0].c_str();

    if (!BCrypt::validatePassword(data["password"], password_hash)) {
        throw std::runtime_error("Password is incorrect!");
    }

    try {
        constexpr auto sql = "INSERT INTO {}(user_id, log_event) VALUES((SELECT id FROM {} WHERE username=$1), $2);";
        auto log = "Have been logged and received JWT token.";
        transaction.exec(fmt::format(sql, user_logs_table_name, user_table_name), pqxx::params(username, log));
        transaction.commit();
    }
    catch (const std::exception& e) {
        transaction.abort();
        throw std::runtime_error("Unknown error!");
    }

    return jwt_manager::create_token(username, "SERVER", std::getenv("JWT_SECRET"));
}