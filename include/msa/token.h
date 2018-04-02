#pragma once

#include <string>
#include <chrono>
#include "scope.h"

class MSAToken {

public:
    using ExpireTime = std::chrono::system_clock::time_point;

protected:
    MSASecurityScope securityScope;
    ExpireTime expireTime;

public:
    MSAToken() { }
    MSAToken(MSASecurityScope const& scope, ExpireTime expire) : securityScope(scope), expireTime(expire) { }

    MSASecurityScope const& getSecurityScope() const { return securityScope; }

    bool isExpired() const { return false; } // TODO:

};