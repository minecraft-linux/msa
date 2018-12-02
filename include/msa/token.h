#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "scope.h"
#include "network/server_time.h"

namespace rapidxml { template <typename T> struct xml_node; }

namespace msa {

enum class TokenType {
    Legacy, Compact
};

class Token {

public:
    using TimePoint = std::chrono::system_clock::time_point;

protected:
    SecurityScope securityScope;
    bool hasLifetimeInfo = false;
    TimePoint createTime, expireTime;

    Token(rapidxml::xml_node<char> const& data);

public:
    Token() { }
    Token(SecurityScope const& scope, TimePoint create, TimePoint expire) : hasLifetimeInfo(true),
        securityScope(scope), createTime(create), expireTime(expire) { }

    virtual ~Token() = default;

    virtual TokenType getType() const = 0;

    SecurityScope const& getSecurityScope() const { return securityScope; }

    TimePoint getCreatedTime() const { return createTime; }

    TimePoint getExpiresTime() const { return expireTime; }

    bool isExpired() const { return network::ServerTime::getServerTime() >= expireTime; }

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