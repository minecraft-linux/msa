#include <msa/token.h>
#include <msa/xml_utils.h>
#include <cstring>
#include <msa/legacy_token.h>
#include <msa/compact_token.h>
#include <ctime>

using namespace msa;
using namespace rapidxml;

Token::Token(rapidxml::xml_node<char> const& data) {
    securityScope = parseSecurityScope(data);

    rapidxml::xml_node<char>* lifetime = data.first_node("wst:Lifetime");
    if (lifetime != nullptr) {
        hasLifetimeInfo = true;

        struct tm tm;
        rapidxml::xml_node<char>* created = lifetime->first_node("wsu:Created");
        if (created != nullptr && strptime(created->value(), "%FT%TZ", &tm))
            createTime = std::chrono::system_clock::from_time_t(timegm(&tm));

        rapidxml::xml_node<char>* expires = lifetime->first_node("wsu:Expires");
        if (expires != nullptr && strptime(expires->value(), "%FT%TZ", &tm))
            expireTime = std::chrono::system_clock::from_time_t(timegm(&tm));
    }
}

void Token::toXml(rapidxml::xml_node<char>& node) const {
    auto& doc = *node.document();

    switch (getType()) {
        case TokenType::Legacy:
            node.append_node(doc.allocate_node(node_element, "wst:TokenType", "urn:passport:legacy"));
            break;
        case TokenType::Compact:
            node.append_node(doc.allocate_node(node_element, "wst:TokenType", "urn:passport:compact"));
            break;
    }

    auto appliesTo = doc.allocate_node(node_element, "wsp:AppliesTo");
    auto appliesToEndpointRef = doc.allocate_node(node_element, "wsa:EndpointReference");
    appliesToEndpointRef->append_node(XMLUtils::allocateNodeCopyValue(doc, "wsa:Address", securityScope.address));
    appliesTo->append_node(appliesToEndpointRef);
    node.append_node(appliesTo);

    if (hasLifetimeInfo) {
        auto lifetime = doc.allocate_node(node_element, "wst:Lifetime");

        struct tm tm;
        char buf[512];
        time_t tt = std::chrono::system_clock::to_time_t(createTime);
        gmtime_r(&tt, &tm);
        strftime(buf, sizeof(buf), "%FT%TZ", &tm);
        lifetime->append_node(XMLUtils::allocateNodeCopyValue(doc, "wsu:Created", buf));
        tt = std::chrono::system_clock::to_time_t(expireTime);
        gmtime_r(&tt, &tm);
        strftime(buf, sizeof(buf), "%FT%TZ", &tm);
        lifetime->append_node(XMLUtils::allocateNodeCopyValue(doc, "wsu:Expires", buf));

        node.append_node(lifetime);
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