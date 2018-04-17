#pragma once

#include <string>
#include <memory>
#include "token.h"

namespace msa {

class TokenErrorInfo;

class TokenResponse {

private:
    SecurityScope securityScope;
    std::shared_ptr<Token> token;
    std::shared_ptr<TokenErrorInfo> error;

public:
    TokenResponse() { }
    TokenResponse(SecurityScope scope, std::shared_ptr<Token> token) : securityScope(scope), token(token) { }
    TokenResponse(SecurityScope scope, std::shared_ptr<TokenErrorInfo> error) : securityScope(scope), error(error) { }

    static TokenResponse fromXml(rapidxml::xml_node<char> const& data);

    bool hasError() const { return token == nullptr; }
    SecurityScope const& getSecurityScope() const { return securityScope; }
    std::shared_ptr<Token> getToken() const { return token; }
    std::shared_ptr<TokenErrorInfo> getError() const { return error; }

};

struct TokenErrorInfo {
    unsigned int reqStatus = 0;
    unsigned int errorStatus = 0;
    std::string flowUrl;
    std::string inlineAuthUrl;
    std::string inlineEndAuthUrl;

    static TokenErrorInfo fromXml(rapidxml::xml_node<char> const& data);
};

}
