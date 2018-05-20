#include <msa/compact_token.h>
#include <msa/xml_utils.h>

using namespace msa;
using namespace rapidxml;

CompactToken::CompactToken(rapidxml::xml_node<char> const& data) : Token(data) {
    auto& tokenData = XMLUtils::getRequiredChild(data, "wst:RequestedSecurityToken");
    auto& binaryData = XMLUtils::getRequiredChild(tokenData, "wsse:BinarySecurityToken");
    this->binaryToken = std::string(binaryData.value(), binaryData.value_size());
}

void CompactToken::toXml(rapidxml::xml_node<char>& node) {
    Token::toXml(node);
    auto& doc = *node.document();
    auto tokenData = doc.allocate_node(node_element, "wst:RequestedSecurityToken");
    tokenData->append_node(XMLUtils::allocateNodeCopyValue(doc, "wsse:BinarySecurityToken", binaryToken));
    node.append_node(tokenData);
}