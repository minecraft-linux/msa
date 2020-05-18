#include <msa/network/security_token_request.h>
#include <msa/network/server_time.h>
#include <msa/xml_utils.h>
#include <cstring>
#include <base64.h>
#include <msa/network/crypto_utils.h>
#include <msa/network/soap_exception.h>
#include <log.h>
#include <stdexcept>

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

const char* const SecurityTokenRequestBase::BINARY_VERSION_STRING = "11";
const char* const SecurityTokenRequestBase::DEVICE_TYPE = "Android";
const char* const SecurityTokenRequestBase::HOSTING_APP = "{F501FD64-9070-46AB-993C-6F7B71D8D883}"; // MSA login library app id;

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

    XMLSignContext signContext;

    auto header = doc.allocate_node(node_element, "s:Header");
    buildHeader(doc, *header, signContext);
    envelope->append_node(header);

    auto body = doc.allocate_node(node_element, "s:Body");
    buildBody(doc, *body, signContext);
    envelope->append_node(body);

    auto signKey = getSigingKey();
    if (signKey != nullptr) {
        header->first_node("wsse:Security")->append_node(signContext.createNonceNode(doc));
        header->first_node("wsse:Security")->append_node(signContext.createSignature(*signKey, doc));
    }

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

void SecurityTokenRequestBase::buildHeader(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header,
                                           XMLSignContext& signContext) const {
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
    signContext.addElement(*authInfo);

    auto security = doc.allocate_node(node_element, "wsse:Security");
    buildHeaderSecurity(doc, *security, signContext);
    header.append_node(security);
}

void SecurityTokenRequestBase::buildHeaderAuthInfo(rapidxml::xml_document<char>& doc,
                                                   rapidxml::xml_node<char>& authInfo) const {
    authInfo.append_node(doc.allocate_node(node_element, "ps:BinaryVersion", BINARY_VERSION_STRING));
    authInfo.append_node(doc.allocate_node(node_element, "ps:DeviceType", DEVICE_TYPE));
    authInfo.append_node(doc.allocate_node(node_element, "ps:HostingApp", HOSTING_APP));
}

void SecurityTokenRequestBase::buildTimestamp(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& parent,
                                              XMLSignContext& signContext) const {
    std::chrono::system_clock::time_point created = ServerTime::getServerTime();

    char createdTimestamp[24];
    time_t time = std::chrono::system_clock::to_time_t(created);
    strftime(createdTimestamp, sizeof(createdTimestamp), "%FT%TZ", gmtime(&time));
    char expiresTimestamp[24];
    time = std::chrono::system_clock::to_time_t(created + std::chrono::minutes(5));
    strftime(expiresTimestamp, sizeof(expiresTimestamp), "%FT%TZ", gmtime(&time));

    auto timestampNode = doc.allocate_node(node_element, "wsu:Timestamp");
    timestampNode->append_attribute(doc.allocate_attribute("xmlns:wsu", NAMESPACE_WSS_UTILITY));
    timestampNode->append_attribute(doc.allocate_attribute("wsu:Id", "Timestamp"));
    timestampNode->append_node(doc.allocate_node(node_element, "wsu:Created", doc.allocate_string(createdTimestamp)));
    timestampNode->append_node(doc.allocate_node(node_element, "wsu:Expires", doc.allocate_string(expiresTimestamp)));
    parent.append_node(timestampNode);
    signContext.addElement(*timestampNode);
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

rapidxml::xml_node<char>& SecurityTokenRequestBase::buildMultipleTokenRequestElement(
        rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body) const {
    auto node = doc.allocate_node(node_element, "ps:RequestMultipleSecurityTokens");
    node->append_attribute(doc.allocate_attribute("xmlns:ps", NAMESPACE_PPCRL));
    node->append_attribute(XMLUtils::allocateAttrCopyValue(doc, "Id", "RSTS"));
    body.append_node(node);
    return *node;
}


SecurityTokenResponse SecurityTokenRequestBase::handleResponse(rapidxml::xml_document<char> const& doc) const {
    auto& envelope = XMLUtils::getRequiredChild(doc, "S:Envelope");
    auto& header = XMLUtils::getRequiredChild(envelope, "S:Header");
    auto& body = XMLUtils::getRequiredChild(envelope, "S:Body");
    LegacyToken* signKey;

    if (body.first_node("S:Fault") != nullptr && (signKey = getSigingKey()) != nullptr) {
        if (header.first_node("wsse:Security") == nullptr)
            throw SoapException::fromResponse(body);

        std::string nonce = findEncKeyNonce(envelope);
        std::string key = CryptoUtils::generateSharedKey(32, signKey->getBinarySecret(),
                                                         "WS-SecureConversationWS-SecureConversation", nonce);

        auto& epp = XMLUtils::getRequiredChild(header, "psf:EncryptedPP");
        auto& encryptedData = XMLUtils::getRequiredChild(epp, "EncryptedData");

        auto& cipherData = XMLUtils::getRequiredChild(encryptedData, "CipherData");
        std::string data = Base64::decode(XMLUtils::getRequiredChildValue(cipherData, "CipherValue"));
        std::string decryptedHeader = CryptoUtils::decryptAES256cbc(data, key);
#ifdef MSA_LOG_NETWORK
        Log::trace("SecurityTokenRequest", "Decrypted header: %s", decryptedHeader.c_str());
#endif

        rapidxml::xml_document<char> ddoc;
        ddoc.parse<0>(&decryptedHeader[0]);

        SecurityTokenResponse ret;
        ret.error = std::make_shared<TokenErrorInfo>(TokenErrorInfo::fromXml(
                XMLUtils::getRequiredChild(ddoc, "psf:pp")));
        return ret;
    }

    auto encryptedData = body.first_node("EncryptedData");
    if (encryptedData != nullptr && (signKey = getSigingKey()) != nullptr) {
        std::string nonce = findEncKeyNonce(envelope);
        std::string key = CryptoUtils::generateSharedKey(32, signKey->getBinarySecret(),
                                                         "WS-SecureConversationWS-SecureConversation", nonce);

        auto& cipherData = XMLUtils::getRequiredChild(*encryptedData, "CipherData");
        std::string data = Base64::decode(XMLUtils::getRequiredChildValue(cipherData, "CipherValue"));
        std::string decryptedBody = CryptoUtils::decryptAES256cbc(data, key);
#ifdef MSA_LOG_NETWORK
        Log::trace("SecurityTokenRequest", "Decrypted body: %s", decryptedBody.c_str());
#endif

        rapidxml::xml_document<char> ddoc;
        ddoc.parse<0>(&decryptedBody[0]);
        return handleResponseBody(ddoc);
    }
    return handleResponseBody(body);
}

SecurityTokenResponse SecurityTokenRequestBase::handleResponseBody(rapidxml::xml_node<char> const& body) const {
    auto singleToken = body.first_node("wst:RequestSecurityTokenResponse");
    if (singleToken != nullptr)
        return {{TokenResponse::fromXml(*singleToken)}};

    auto tokenCollection = body.first_node("wst:RequestSecurityTokenResponseCollection");
    if (tokenCollection != nullptr) {
        std::vector<TokenResponse> ret;
        for (auto it = tokenCollection->first_node("wst:RequestSecurityTokenResponse"); it != nullptr;
                it = it->next_sibling("wst:RequestSecurityTokenResponse")) {
            ret.push_back(TokenResponse::fromXml(*it));
        }
        return {std::move(ret)};
    }

    throw std::runtime_error("Invalid body: no token response");
}

std::string SecurityTokenRequestBase::findEncKeyNonce(rapidxml::xml_node<char> const& envelope) const {
    auto& header = XMLUtils::getRequiredChild(envelope, "S:Header");
    auto& security = XMLUtils::getRequiredChild(header, "wsse:Security");
    for (auto it = security.first_node("wssc:DerivedKeyToken"); it != nullptr;
         it = it->next_sibling("wssc:DerivedKeyToken")) {
        rapidxml::xml_attribute<char>* attr = it->first_attribute("wsu:Id");
        if (attr != nullptr && strcmp(attr->value(), "EncKey") == 0)
            return Base64::decode(XMLUtils::getRequiredChildValue(*it, "wssc:Nonce"));
    }
    return std::string();
}