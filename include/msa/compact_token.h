#pragma once

#include "token.h"

namespace msa {

class CompactToken : public Token {

private:
    std::string binaryToken;

public:
    CompactToken(std::string const& binaryToken) : binaryToken(binaryToken) { }

    CompactToken(SecurityScope const& scope, ExpireTime expire, std::string const& binaryToken) :
            Token(scope, expire), binaryToken(binaryToken) { }

    std::string const& getBinaryToken() const { return binaryToken; }

};

}