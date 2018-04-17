#include <msa/token_response.h>

using namespace msa;

TokenResponse TokenResponse::fromXml(rapidxml::xml_node<char> const& data) {
    try {
        auto token = Token::fromXml(data);
        return TokenResponse(token->getSecurityScope(), token);
    } catch (std::exception& e) {
        SecurityScope scope;
        try {
            scope = Token::parseSecurityScope(data);
        } catch (std::exception&) {
        }
        std::shared_ptr<TokenErrorInfo> error(new TokenErrorInfo(TokenErrorInfo::fromXml(data)));
        return TokenResponse(scope, error);
    }
}

TokenErrorInfo TokenErrorInfo::fromXml(rapidxml::xml_node<char> const& data) {
    // TODO: Implementation
    return TokenErrorInfo();
}