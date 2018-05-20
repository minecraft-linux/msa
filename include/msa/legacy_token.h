#pragma once

#include "token.h"

namespace msa {

class LegacyToken : public Token {

private:
    std::string xmlData;
    std::string binarySecret;

public:
    LegacyToken(std::string const& xmlData, std::string const& key) : xmlData(xmlData), binarySecret(key) { }

    LegacyToken(SecurityScope const& scope, ExpireTime expire, std::string const& xmlData,
                std::string const& key) : Token(scope, expire), xmlData(xmlData), binarySecret(key) { }

    LegacyToken(rapidxml::xml_node<char> const& data);

    TokenType getType() const override { return TokenType::Legacy; }

    void toXml(rapidxml::xml_node<char>& node) override;

    std::string const& getXmlData() const { return xmlData; }
    std::string const& getBinarySecret() const { return binarySecret; }

};

template <>
inline LegacyToken& token_cast(Token& t) {
    if (t.getType() != TokenType::Legacy)
        throw std::bad_cast();
    return (LegacyToken&) t;
}

template <>
inline std::shared_ptr<LegacyToken> token_pointer_cast(std::shared_ptr<Token> t) {
    if (t->getType() != TokenType::Legacy)
        throw std::bad_cast();
    return std::static_pointer_cast<LegacyToken>(t);
}

template <>
inline LegacyToken const& token_cast(Token const& t) {
    if (t.getType() != TokenType::Legacy)
        throw std::bad_cast();
    return (LegacyToken const&) t;
}


}