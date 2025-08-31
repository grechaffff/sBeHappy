#pragma once

#include <string>

struct password_manager {
    enum class complexity_e {
        invalid, easy, medium, hard
    };

    static complexity_e check(const std::string& password) {
        int lowercase_letters = 0,
            uppercase_letters = 0,
            digits = 0,
            underscores = 0;
        
        for (auto c : password) {
            if (c >= 'a' && c <= 'z')
                ++lowercase_letters;
            else if (c >= 'A' && c <= 'Z')
                ++uppercase_letters;
            else if (c >= '0' && c <= '9')
                ++digits;
            else if (c == '_')
                ++underscores;
            else
                return complexity_e::invalid;
        }

        int lenght = password.size();

        if (lenght >= 12 && lowercase_letters >= 2 && uppercase_letters >= 2 && digits >= 2 && underscores >= 1)
            return complexity_e::hard;
        else if (lenght >= 8 && lowercase_letters >= 1 && uppercase_letters >= 1 && digits >= 1)
            return complexity_e::medium;
        else
            return complexity_e::easy;
    }
};
