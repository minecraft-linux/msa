#pragma once

#include <exception>
#include <string>

namespace msa {

class ParseException : public std::exception {

private:
    std::string message;

public:
    ParseException(std::string message) : message(std::move(message)) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

};

}