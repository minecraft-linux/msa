#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "scope.h"

namespace rapidxml { template <typename T> struct xml_node; }

namespace msa {

enum class TokenType {
    Legacy, Compact
};

class Token {

public:
    using ExpireTime = std::chrono::system_clock::time_point;

protected:
    SecurityScope securityScope;
    bool hasLifetimeInfo = false;
    ExpireTime createTime, expireTime;

    Token(rapidxml::xml_node<char> const& data);

public:
    Token() { }
    Token(SecurityScope const& scope, ExpireTime expire) : securityScope(scope), expireTime(expire) { }

    virtual ~Token() = default;

    virtual TokenType getType() const = 0;

    SecurityScope const& getSecurityScope() const { return securityScope; }

    bool isExpired() const { return false; } // TODO:

    virtual void toXml(rapidxml::xml_node<char>& node) const;


    static std::shared_ptr<Token> fromXml(rapidxml::xml_node<char> const& data);

    static SecurityScope parseSecurityScope(rapidxml::xml_node<char> const& data);

};

template <typename Ret>
Ret& token_cast(Token&);

template <typename Ret>
std::shared_ptr<Ret> token_pointer_cast(std::shared_ptr<Token>);

template <typename Ret>
Ret const& token_cast(Token const&);

}