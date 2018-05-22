#pragma once

#include "token.h"

namespace msa {

class CompactToken : public Token {

private:
    std::string binaryToken;

public:
    CompactToken(std::string const& binaryToken) : binaryToken(binaryToken) { }

    CompactToken(SecurityScope const& scope, TimePoint create, TimePoint expire, std::string const& binaryToken) :
            Token(scope, create, expire), binaryToken(binaryToken) { }

    CompactToken(rapidxml::xml_node<char> const& data);

    TokenType getType() const override { return TokenType::Compact; }

    void toXml(rapidxml::xml_node<char>& node) const override;

    std::string const& getBinaryToken() const { return binaryToken; }

};

template <>
inline CompactToken& token_cast(Token& t) {
    if (t.getType() != TokenType::Compact)
        throw std::bad_cast();
    return (CompactToken&) t;
}

template <>
inline std::shared_ptr<CompactToken> token_pointer_cast(std::shared_ptr<Token> t) {
    if (t->getType() != TokenType::Compact)
        throw std::bad_cast();
    return std::static_pointer_cast<CompactToken>(t);
}

template <>
inline CompactToken const& token_cast(Token const& t) {
    if (t.getType() != TokenType::Compact)
        throw std::bad_cast();
    return (CompactToken const&) t;
}

}