#pragma once

#include <string>
#include <chrono>
#include "scope.h"

namespace msa {

class Token {

public:
    using ExpireTime = std::chrono::system_clock::time_point;

protected:
    SecurityScope securityScope;
    ExpireTime expireTime;

public:
    Token() { }
    Token(SecurityScope const& scope, ExpireTime expire) : securityScope(scope), expireTime(expire) { }

    SecurityScope const& getSecurityScope() const { return securityScope; }

    bool isExpired() const { return false; } // TODO:

};

}