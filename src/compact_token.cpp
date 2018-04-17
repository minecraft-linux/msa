#include <msa/compact_token.h>
#include <msa/xml_utils.h>

using namespace msa;

CompactToken::CompactToken(rapidxml::xml_node<char> const& data) : Token(data) {
    auto& tokenData = XMLUtils::getRequiredChild(data, "wst:RequestedSecurityToken");
    auto& binaryData = XMLUtils::getRequiredChild(tokenData, "wsse:BinarySecurityToken");
    this->binaryToken = std::string(binaryData.value(), binaryData.value_size());
}