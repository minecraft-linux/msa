#include <msa/network/device_authenticate_request.h>
#include <msa/parse_exception.h>

using namespace msa::network;
using namespace rapidxml;

void DeviceAuthenticateRequest::buildHeaderSecurity(rapidxml::xml_document<char>& doc,
                                                    rapidxml::xml_node<char>& header) const {
    auto usernameToken = doc.allocate_node(node_element, "wsse:UsernameToken");
    usernameToken->append_attribute(doc.allocate_attribute("wsu:Id", "devicesoftware"));

    usernameToken->append_node(doc.allocate_node(node_element, "wsse:Username", username.c_str(), 0, username.length()));
    usernameToken->append_node(doc.allocate_node(node_element, "wsse:Password", password.c_str(), 0, password.length()));

    header.append_node(usernameToken);

    buildTimestamp(doc, header);
}

void DeviceAuthenticateRequest::buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body) const {
    buildTokenRequest(doc, body, {"http://Passport.NET/tb"});
}

DeviceAuthenticateResponse DeviceAuthenticateRequest::handleResponse(SecurityTokenResponse const& resp) const {
    if (resp.tokens.size() <= 0)
        throw ParseException("No token returned");
    auto const& rtoken = resp.tokens[0];
    if (rtoken.hasError())
        throw ParseException("Token has error: " + std::to_string(rtoken.getError()->errorStatus));
    auto token = rtoken.getToken();
    if (token == nullptr || token->getType() != TokenType::Legacy)
        throw ParseException("Invalid token received");
    return {std::dynamic_pointer_cast<LegacyToken>(token)};
}
