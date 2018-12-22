#pragma once

#include <exception>
#include <string>

namespace rapidxml { template<class Ch> class xml_node; }

namespace msa {
namespace network {

class SoapException : public std::exception {

private:
    std::string errorCode, errorSubcode, errorReason;

    std::string createString() const;

protected:
    std::string asString;

    explicit SoapException(rapidxml::xml_node<char> const& doc);

public:
    SoapException(std::string code, std::string subcode, std::string reason) : errorCode(std::move(code)),
        errorSubcode(std::move(subcode)), errorReason(std::move(reason)) {
        asString = createString();
    }

    static SoapException fromResponse(rapidxml::xml_node<char> const& doc);

    inline std::string const& code() const { return errorCode; }

    inline std::string const& subcode() const { return errorSubcode; }

    const char* what() const noexcept override {
        return asString.c_str();
    }

};

}
}
