#include <msa/legacy_token.h>
#include <msa/xml_utils.h>
#include <rapidxml_print.hpp>
#include <sstream>
#include <base64.h>

using namespace msa;

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