#pragma once

#include <exception>
#include <string>

namespace rapidxml { template<class Ch> class xml_node; }

namespace msa {
namespace network {

class ServiceException : public std::exception {

private:
    const int errorCode;
    const long long errorSubcode;
    std::string asString;

    std::string createString() const;

public:
    ServiceException(int code, long long subcode) : errorCode(code), errorSubcode(subcode) {
        asString = createString();
    }

    static ServiceException fromResponse(rapidxml::xml_node<char> const& doc);

    inline int code() const { return errorCode; }

    inline long long subcode() const { return errorSubcode; }

    const char* what() const noexcept override {
        return asString.c_str();
    }

};

}
}
