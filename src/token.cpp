#include <msa/token.h>
#include <msa/xml_utils.h>
#include <cstring>
#include <msa/legacy_token.h>
#include <msa/compact_token.h>

using namespace msa;

Token::Token(rapidxml::xml_node<char> const& data) {
    securityScope = parseSecurityScope(data);

    Token::ExpireTime expire;
    rapidxml::xml_node<char>* lifetime = data.first_node("wst:Lifetime");
    if (lifetime != nullptr) {
        rapidxml::xml_node<char>* expires = data.first_node("wsu:Expires");
        struct tm tm;
        if (expires != nullptr && strptime(expires->value(), "%FT%TZ", &tm))
            expire = std::chrono::system_clock::from_time_t(timegm(&tm));
    }
}

SecurityScope Token::parseSecurityScope(rapidxml::xml_node<char> const& data) {
    auto& appliesTo = XMLUtils::getRequiredChild(data, "wsp:AppliesTo");
    auto& appliesToEndpointRef = XMLUtils::getRequiredChild(appliesTo, "wsa:EndpointReference");
    auto& address = XMLUtils::getRequiredChild(appliesToEndpointRef, "wsa:Address");
    return {std::string(address.value(), address.value_size())};
}

std::shared_ptr<Token> Token::fromXml(rapidxml::xml_node<char> const& data) {
    auto tokenType = XMLUtils::getRequiredChild(data, "wst:TokenType").value();

    if (strcmp(tokenType, "urn:passport:legacy") == 0)
        return std::shared_ptr<LegacyToken>(new LegacyToken(data));
    else if (strcmp(tokenType, "urn:passport:compact") == 0)
        return std::shared_ptr<CompactToken>(new CompactToken(data));
    throw ParseException("Invalid token type");
}