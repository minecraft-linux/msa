#include <msa/network/account_token_request.h>

#include <map>
#include <msa/network/request_utils.h>
#include <msa/network/server_time.h>
#include <msa/xml_utils.h>
#include <base64.h>
#include <msa/network/crypto_utils.h>

using namespace msa::network;
using namespace rapidxml;

void AccountTokenRequest::buildHeaderAuthInfo(rapidxml::xml_document<char>& doc,
                                              rapidxml::xml_node<char>& authInfo) const {
    SecurityTokenRequest::buildHeaderAuthInfo(doc, authInfo);
    authInfo.append_node(doc.allocate_node(node_element, "ps:InlineUX", DEVICE_TYPE));
    authInfo.append_node(doc.allocate_node(node_element, "ps:ConsentFlags", "1"));
    authInfo.append_node(doc.allocate_node(node_element, "ps:IsConnected", "1"));
    if (clientAppUri.length() > 0)
        authInfo.append_node(doc.allocate_node(node_element, "ps:ClientAppURI", clientAppUri.c_str(), 0, clientAppUri.length()));
}

void AccountTokenRequest::buildHeaderSecurity(rapidxml::xml_document<char>& doc,
                                              rapidxml::xml_node<char>& header) const {
    {
        rapidxml::xml_document<char> daTokenDoc;
        daTokenDoc.parse<rapidxml::parse_non_destructive>((char*) daToken->getXmlData().c_str());
        header.append_node(doc.clone_node(daTokenDoc.first_node()));
    }

    auto binarySecurityToken = XMLUtils::allocateNodeCopyValue(doc, "wsse:BinarySecurityToken", generateDeviceProofUri());
    binarySecurityToken->append_attribute(doc.allocate_attribute("ValueType", "urn:liveid:sha1device"));
    binarySecurityToken->append_attribute(doc.allocate_attribute("Id", "DeviceDAToken"));
    header.append_node(binarySecurityToken);
}

std::string AccountTokenRequest::generateDeviceProofUri() const {
    using namespace std::chrono;

    std::vector<std::pair<std::string, std::string>> values;
    std::string nonce = CryptoUtils::createNonce();
    values.emplace_back("ct", std::to_string(duration_cast<seconds>(ServerTime::getServerTime().time_since_epoch()).count()));
    values.emplace_back("hashalg", "SHA256");
    values.emplace_back("bver", BINARY_VERSION_STRING);
    values.emplace_back("appid", HOSTING_APP);
    values.emplace_back("da", deviceToken->getXmlData());
    values.emplace_back("nonce", Base64::encode(nonce));

    values.emplace_back("hash", CryptoUtils::sign(RequestUtils::encodeUrlParams(values),
                                                  deviceToken->getBinarySecret(), "WS-SecureConversation", nonce));

    return RequestUtils::encodeUrlParams(values);
}

void AccountTokenRequest::buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body) const {
    auto ctr = &body;
    if (scopes.size() > 1)
        ctr = &buildMultipleTokenRequestElement(doc, body);

    int index = 0;
    for (auto const& scope : scopes) {
        buildTokenRequest(doc, *ctr, scope, index++);
    }
}