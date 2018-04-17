#pragma once

#include <string>
#include <rapidxml.hpp>
#include "parse_exception.h"

namespace msa {
namespace network {

class XMLUtils {

public:
    static rapidxml::xml_node<char> const& getRequiredChild(rapidxml::xml_node<char> const& parent,
                                                            const char* name) {
        auto node = parent.first_node(name);
        if (node == nullptr)
            throw ParseException(std::string("Missing `") + name + "` node");
        return *node;
    }

    static const char* getAttribute(rapidxml::xml_node<char> const& parent, const char* name,
                                    const char* defRet = nullptr) {
        auto node = parent.first_attribute(name);
        if (node == nullptr)
            return defRet;
        return node->value();
    }

    static const char* docCopyString(rapidxml::xml_document<char>& doc, std::string const& value) {
        return doc.allocate_string(value.data(), value.length());
    }

    static rapidxml::xml_node<char>* allocateNodeCopyValue(rapidxml::xml_document<char>& doc, const char* key,
                                                           std::string const& value) {
        return doc.allocate_node(rapidxml::node_element, key, docCopyString(doc, value), 0, value.length());
    }

    static rapidxml::xml_attribute<char>* allocateAttrCopyValue(rapidxml::xml_document<char>& doc, const char* key,
                                                                std::string const& value) {
        return doc.allocate_attribute(key, docCopyString(doc, value), 0, value.length());
    }

};

}
}