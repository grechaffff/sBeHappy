#pragma once

#include <pqxx/pqxx>

#include <string>
#include <optional>

class pqxx_wrapper {
private:
    pqxx::connection connection;

    // not used
    static std::string db_setting(std::string db_name, std::optional<std::string> password, std::optional<std::string> other_setting) {
        std::string setting = "dbname = " + db_name + " ";

        if (password.has_value())
            setting += "password = " + *password + " ";

        if (other_setting.has_value())
            setting += *other_setting;

        return setting;
    }

public:
    pqxx_wrapper(const std::string& setting) : connection(setting) {}

    pqxx_wrapper(const pqxx_wrapper&) = delete;
    pqxx_wrapper& operator=(const pqxx_wrapper&) = delete;

    pqxx::work get_transaction() {
        return pqxx::work(connection); 
    }
};

using database = pqxx_wrapper;
