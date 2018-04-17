#include <msa/network/security_token_request.h>
#include <msa/network/server_time.h>
#include <msa/xml_utils.h>

using namespace msa::network;
using namespace rapidxml;

const char* const SecurityTokenRequestBase::NAMESPACE_SOAP = "http://www.w3.org/2003/05/soap-envelope";
const char* const SecurityTokenRequestBase::NAMESPACE_PPCRL = "http://schemas.microsoft.com/Passport/SoapServices/PPCRL";
const char* const SecurityTokenRequestBase::NAMESPACE_WSS_SECEXT = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd";
const char* const SecurityTokenRequestBase::NAMESPACE_SAML = "urn:oasis:names:tc:SAML:1.0:assertion";
const char* const SecurityTokenRequestBase::NAMESPACE_WS_POLICY = "http://schemas.xmlsoap.org/ws/2004/09/policy";
const char* const SecurityTokenRequestBase::NAMESPACE_WSS_UTILITY = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd";
const char* const SecurityTokenRequestBase::NAMESPACE_WS_ADDRESSING = "http://www.w3.org/2005/08/addressing";
const char* const SecurityTokenRequestBase::NAMESPACE_WS_SECURECONVERSATION = "http://schemas.xmlsoap.org/ws/2005/02/sc";
const char* const SecurityTokenRequestBase::NAMESPACE_WS_TRUST = "http://schemas.xmlsoap.org/ws/2005/02/trust";

SecurityTokenRequestBase::SecurityTokenRequestBase() {
    using namespace std::chrono;
    messageId = std::to_string(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

SecurityTokenResponse SecurityTokenRequestBase::sendTokenRequestInternal() const {
    rapidxml::xml_document<char> doc;
    std::string str = sendInternal();
    doc.parse<0>(&str[0]);
    return handleResponse(doc);
}

void SecurityTokenRequestBase::createBody(rapidxml::xml_document<char>& doc) const {
    auto envelope = doc.allocate_node(node_element, "s:Envelope");
    appendNamespaces(doc, *envelope);

    auto header = doc.allocate_node(node_element, "s:Header");
    buildHeader(doc, *header);
    envelope->append_node(header);

    auto body = doc.allocate_node(node_element, "s:Body");
    buildBody(doc, *body);
    envelope->append_node(body);

    doc.append_node(envelope);
}

void SecurityTokenRequestBase::appendNamespaces(rapidxml::xml_document<char>& doc,
                                                rapidxml::xml_node<char>& envelope) const {
    envelope.append_attribute(doc.allocate_attribute("xmlns:s", NAMESPACE_SOAP));
    envelope.append_attribute(doc.allocate_attribute("xmlns:ps", NAMESPACE_PPCRL));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wsse", NAMESPACE_WSS_SECEXT));
    envelope.append_attribute(doc.allocate_attribute("xmlns:saml", NAMESPACE_SAML));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wsp", NAMESPACE_WS_POLICY));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wsu", NAMESPACE_WSS_UTILITY));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wsa", NAMESPACE_WS_ADDRESSING));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wssc", NAMESPACE_WS_SECURECONVERSATION));
    envelope.append_attribute(doc.allocate_attribute("xmlns:wst", NAMESPACE_WS_TRUST));
}

void SecurityTokenRequestBase::buildHeader(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header) const {
    auto action = doc.allocate_node(node_element, "wsa:Action", "http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue");
    action->append_attribute(doc.allocate_attribute("s:mustUnderstand", "1"));
    header.append_node(action);

    auto to = doc.allocate_node(node_element, "wsa:To", getUrl().c_str());
    to->append_attribute(doc.allocate_attribute("s:mustUnderstand", "1"));
    header.append_node(to);

    header.append_node(doc.allocate_node(node_element, "wsa:MessageID", messageId.data(), 0, messageId.length()));

    auto authInfo = doc.allocate_node(node_element, "ps:AuthInfo");
    authInfo->append_attribute(doc.allocate_attribute("xmlns:ps", NAMESPACE_PPCRL));
    authInfo->append_attribute(doc.allocate_attribute("Id", "PPAuthInfo"));
    buildHeaderAuthInfo(doc, *authInfo);
    header.append_node(authInfo);

    auto security = doc.allocate_node(node_element, "wsse:Security");
    buildHeaderSecurity(doc, *security);
    header.append_node(security);
}

void SecurityTokenRequestBase::buildHeaderAuthInfo(rapidxml::xml_document<char>& doc,
                                                   rapidxml::xml_node<char>& authInfo) const {
    authInfo.append_node(doc.allocate_node(node_element, "ps:BinaryVersion", "11"));
    authInfo.append_node(doc.allocate_node(node_element, "ps:DeviceType", "Android"));
    authInfo.append_node(doc.allocate_node(node_element, "ps:HostingApp", "{F501FD64-9070-46AB-993C-6F7B71D8D883}")); // MSA login library app id
}

void SecurityTokenRequestBase::buildTimestamp(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& parent) const {
    std::chrono::system_clock::time_point created = ServerTime::getServerTime();

    char createdTimestamp[24];
    time_t time = std::chrono::system_clock::to_time_t(created);
    strftime(createdTimestamp, sizeof(createdTimestamp), "%FT%TZ", gmtime(&time));
    char expiresTimestamp[24];
    time = std::chrono::system_clock::to_time_t(created + std::chrono::minutes(5));
    strftime(expiresTimestamp, sizeof(expiresTimestamp), "%FT%TZ", gmtime(&time));

    auto timestampNode = doc.allocate_node(node_element, "wsu:Timestamp");
    timestampNode->append_node(doc.allocate_node(node_element, "wsu:Created", doc.allocate_string(createdTimestamp)));
    timestampNode->append_node(doc.allocate_node(node_element, "wsu:Expires", doc.allocate_string(expiresTimestamp)));
    parent.append_node(timestampNode);
}

void SecurityTokenRequestBase::buildTokenRequest(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body,
                                                 SecurityScope const& scope, int index) const {
    auto token = doc.allocate_node(node_element, "wst:RequestSecurityToken");
    token->append_attribute(doc.allocate_attribute("xmlns:wst", NAMESPACE_WS_TRUST));
    token->append_attribute(XMLUtils::allocateAttrCopyValue(doc, "Id", "RST" + std::to_string(index)));

    token->append_node(doc.allocate_node(node_element, "wst:RequestType", "http://schemas.xmlsoap.org/ws/2005/02/trust/Issue"));

    auto appliesTo = doc.allocate_node(node_element, "wsp:AppliesTo");
    appliesTo->append_attribute(doc.allocate_attribute("xmlns:wsp", NAMESPACE_WS_POLICY));

    auto endpointRef = doc.allocate_node(node_element, "wsa:EndpointReference");
    endpointRef->append_attribute(doc.allocate_attribute("xmlns:wsa", NAMESPACE_WS_ADDRESSING));
    endpointRef->append_node(XMLUtils::allocateNodeCopyValue(doc, "wsa:Address", scope.address));
    appliesTo->append_node(endpointRef);

    token->append_node(appliesTo);

    if (!scope.policyRef.empty()) {
        auto policyRef = doc.allocate_node(node_element, "wsp:PolicyReference");
        policyRef->append_attribute(doc.allocate_attribute("xmlns:wsp", NAMESPACE_WS_POLICY));
        policyRef->append_attribute(XMLUtils::allocateAttrCopyValue(doc, "URI", scope.policyRef));
        token->append_node(policyRef);
    }

    body.append_node(token);
}


SecurityTokenResponse SecurityTokenRequestBase::handleResponse(rapidxml::xml_document<char> const& doc) const {
    auto& envelope = XMLUtils::getRequiredChild(doc, "S:Envelope");
    auto& body = XMLUtils::getRequiredChild(envelope, "S:Body");

    auto singleToken = body.first_node("wst:RequestSecurityTokenResponse");
    if (singleToken != nullptr) {
        return {{TokenResponse::fromXml(*singleToken)}};
    }
}