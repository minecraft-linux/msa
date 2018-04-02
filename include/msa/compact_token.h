#pragma once

#include "token.h"

class MSACompactToken : public MSAToken {

private:
    std::string binaryToken;

public:
    MSACompactToken(std::string const& binaryToken) : binaryToken(binaryToken) { }

    MSACompactToken(MSASecurityScope const& scope, ExpireTime expire, std::string const& binaryToken) :
            MSAToken(scope, expire), binaryToken(binaryToken) { }

    std::string const& getBinaryToken() const { return binaryToken; }

};
