#include <msa/legacy_token.h>
#include <msa/xml_utils.h>
#include <rapidxml_print.hpp>
#include <sstream>
#include <base64.h>

using namespace msa;
using namespace rapidxml;

LegacyToken::LegacyToken(rapidxml::xml_node<char> const& data) : Token(data) {
    auto& tokenData = XMLUtils::getRequiredChild(data, "wst:RequestedSecurityToken");
    auto& encryptedData = XMLUtils::getRequiredChild(tokenData, "EncryptedData");

    std::stringstream ss;
    rapidxml::print_to_stream(ss, encryptedData, rapidxml::print_no_indenting);
    this->xmlData = ss.str();


    auto& proof = XMLUtils::getRequiredChild(data, "wst:RequestedProofToken");
    auto& binarySecret = XMLUtils::getRequiredChild(proof, "wst:BinarySecret");
    this->binarySecret = Base64::decode(std::string(binarySecret.value(), binarySecret.value_size()));
}

void LegacyToken::toXml(rapidxml::xml_node<char>& node) const {
    Token::toXml(node);
    auto& doc = *node.document();
    auto tokenData = doc.allocate_node(node_element, "wst:RequestedSecurityToken");
    {
        rapidxml::xml_document<char> xmlDataDoc;
        xmlDataDoc.parse<rapidxml::parse_non_destructive>((char*) xmlData.c_str());
        tokenData->append_node(doc.clone_node(xmlDataDoc.first_node()));
    }
    node.append_node(tokenData);
    auto proof = doc.allocate_node(node_element, "wst:RequestedProofToken");
    proof->append_node(XMLUtils::allocateNodeCopyValue(doc, "wst:BinarySecret", Base64::encode(binarySecret)));
    node.append_node(proof);
}