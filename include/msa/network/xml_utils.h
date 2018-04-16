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

};

}
}