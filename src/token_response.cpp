#include <msa/token_response.h>
#include <rapidxml.hpp>

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
    TokenErrorInfo ret;
    rapidxml::xml_node<char>* reqStatus = data.first_node("psf:reqstatus");
    if (reqStatus != nullptr)
        ret.reqStatus = (unsigned int) std::atoi(reqStatus->value());
    rapidxml::xml_node<char>* errorStatus = data.first_node("psf:errorstatus");
    if (errorStatus != nullptr)
        ret.errorStatus = (unsigned int) std::atoi(errorStatus->value());
    rapidxml::xml_node<char>* flowUrl = data.first_node("psf:flowurl");
    if (flowUrl != nullptr)
        ret.flowUrl = flowUrl->value();
    rapidxml::xml_node<char>* inlineAuthUrl = data.first_node("psf:inlineauthurl");
    if (inlineAuthUrl != nullptr)
        ret.inlineAuthUrl = inlineAuthUrl->value();
    rapidxml::xml_node<char>* inlineEndAuthUrl = data.first_node("psf:inlineendauthurl");
    if (inlineEndAuthUrl != nullptr)
        ret.inlineEndAuthUrl = inlineEndAuthUrl->value();
    return ret;
}