#pragma once

#include <string>
#include <vector>
#include <rapidxml.hpp>
#include "../legacy_token.h"

namespace msa {
namespace network {

class XMLSignContext {

private:
    static const char* const NAMESPACE_XMLDSIG;

    std::vector<rapidxml::xml_node<char>*> nodes;
    std::string nonce;

    rapidxml::xml_node<char>* buildSignatureNode(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& ofNode);

public:
    void addElement(rapidxml::xml_node<char>& node) {
        nodes.push_back(&node);
    }

    std::string const& getNonce();

    rapidxml::xml_node<char>* createNonceNode(rapidxml::xml_document<char>& doc);

    rapidxml::xml_node<char>* createSignature(LegacyToken& daToken, rapidxml::xml_document<char>& doc);

};

}
}