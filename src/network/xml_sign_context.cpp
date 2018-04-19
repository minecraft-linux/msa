#include <msa/network/xml_sign_context.h>

#include <sstream>
#include <msa/network/crypto_utils.h>
#include <rapidxml_print.hpp>
#include <base64.h>
#include <msa/xml_utils.h>

using namespace msa::network;
using namespace rapidxml;

const char* const XMLSignContext::NAMESPACE_XMLDSIG = "http://www.w3.org/2000/09/xmldsig#";

std::string const& XMLSignContext::getNonce() {
    if (nonce.empty())
        nonce = CryptoUtils::createNonce();
    return nonce;
}

rapidxml::xml_node<char>* XMLSignContext::createSignature(LegacyToken& daToken, rapidxml::xml_document<char>& doc) {
    auto signedInfo = doc.allocate_node(node_element, "SignedInfo");
    signedInfo->append_attribute(doc.allocate_attribute("xmlns", NAMESPACE_XMLDSIG));
    auto canMethod = doc.allocate_node(node_element, "CanonicalizationMethod");
    canMethod->append_attribute(doc.allocate_attribute("Algorithm", "http://www.w3.org/2001/10/xml-exc-c14n#"));
    signedInfo->append_node(canMethod);
    auto sigMethod = doc.allocate_node(node_element, "SignatureMethod");
    sigMethod->append_attribute(doc.allocate_attribute("Algorithm", "http://www.w3.org/2001/04/xmldsig-more#hmac-sha256"));
    signedInfo->append_node(sigMethod);
    for (auto const& signNode : nodes) {
        signedInfo->append_node(buildSignatureNode(doc, *signNode));
    }

    auto signature = doc.allocate_node(node_element, "SignedInfo");
    signature->append_node(signedInfo);

    std::string signedInfoStr = XMLUtils::printXmlToString(*signature, print_no_indenting);
    std::string sigValue = CryptoUtils::sign(signedInfoStr, daToken.getBinarySecret(), "WS-SecureConversationWS-SecureConversation", nonce);
    signature->append_node(XMLUtils::allocateNodeCopyValue(doc, "SignatureValue", sigValue));

    auto keyInfo = doc.allocate_node(node_element, "KeyInfo");
    auto tokenRefCtr = doc.allocate_node(node_element, "wsse:SecurityTokenReference");
    auto tokenRef = doc.allocate_node(node_element, "wsse:Reference");
    tokenRef->append_attribute(doc.allocate_attribute("URI", "#SignKey"));
    tokenRefCtr->append_node(tokenRef);
    keyInfo->append_node(tokenRefCtr);
    signature->append_node(keyInfo);

    return signature;
}

rapidxml::xml_node<char>* XMLSignContext::buildSignatureNode(rapidxml::xml_document<char>& doc,
                                                             rapidxml::xml_node<char>& ofNode) {
    std::string hash = CryptoUtils::sha256(XMLUtils::printXmlToString(ofNode, print_no_indenting));

    auto refNode = doc.allocate_node(node_element, "Reference");

    auto transformsNode = doc.allocate_node(node_element, "Transforms");
    auto transformNode = doc.allocate_node(node_element, "Transform");
    transformNode->append_attribute(doc.allocate_attribute("Algorithm", "http://www.w3.org/2001/10/xml-exc-c14n#"));
    transformsNode->append_node(transformNode);
    refNode->append_node(transformsNode);

    auto digestMethodNode = doc.allocate_node(node_element, "DigestMethod");
    digestMethodNode->append_attribute(doc.allocate_attribute("Algorithm", "http://www.w3.org/2001/04/xmlenc#sha256"));
    refNode->append_node(digestMethodNode);

    auto digestValueNode = XMLUtils::allocateNodeCopyValue(doc, "DigestValue", Base64::encode(hash));
    refNode->append_node(digestValueNode);

    return refNode;
}