#pragma once

#include <pqxx/pqxx>

#include <string>
#include <optional>

class pqxx_wrapper {
private:
    pqxx::connection connection;
    pqxx::work transaction;

    static std::string db_setting(std::string db_name, std::optional<std::string> password, std::optional<std::string> other_setting) {
        std::string setting = "dbname = " + db_name + " ";

        if (password.has_value())
            setting += "password = " + *password + " ";

        if (other_setting.has_value())
            setting += *other_setting;

        return setting;
    }

public:
    pqxx_wrapper(std::string db_name, std::optional<std::string> password = std::nullopt, 
        std::optional<std::string> other_setting = std::nullopt)
        : connection(db_setting(db_name, password, other_setting))
        , transaction(connection) {}

    pqxx_wrapper(const pqxx_wrapper&) = delete;
    pqxx_wrapper& operator=(const pqxx_wrapper&) = delete;

    pqxx::work& get_transaction() {
        return transaction; 
    }
    const pqxx::work& get_transaction() const {
        return transaction; 
    }

    pqxx::work& operator()() {
        return transaction;
    }

    void commit() {
        transaction.commit();
    }
};

using database = pqxx_wrapper;
