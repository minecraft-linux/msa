#pragma once

#include "token.h"

class MSALegacyToken : public MSAToken {

private:
    std::string xmlData;
    std::string binarySecret;

public:
    MSALegacyToken(std::string const& xmlData, std::string const& key) : xmlData(xmlData), binarySecret(key) { }

    MSALegacyToken(MSASecurityScope const& scope, ExpireTime expire, std::string const& xmlData,
                   std::string const& key) : MSAToken(scope, expire), xmlData(xmlData), binarySecret(key) { }

    std::string const& getXmlData() const { return xmlData; }
    std::string const& getBinarySecret() const { return binarySecret; }

};