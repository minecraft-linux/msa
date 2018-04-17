#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "scope.h"

namespace rapidxml { template <typename T> struct xml_node; }

namespace msa {

class Token {

public:
    using ExpireTime = std::chrono::system_clock::time_point;

protected:
    SecurityScope securityScope;
    ExpireTime expireTime;

    Token(rapidxml::xml_node<char> const& data);

public:
    Token() { }
    Token(SecurityScope const& scope, ExpireTime expire) : securityScope(scope), expireTime(expire) { }

    static std::shared_ptr<Token> fromXml(rapidxml::xml_node<char> const& data);

    static SecurityScope parseSecurityScope(rapidxml::xml_node<char> const& data);

    SecurityScope const& getSecurityScope() const { return securityScope; }

    bool isExpired() const { return false; } // TODO:

};

}