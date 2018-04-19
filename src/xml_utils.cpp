#include <msa/xml_utils.h>
#include <sstream>
#include <rapidxml_print.hpp>

using namespace msa;

std::string XMLUtils::printXmlToString(rapidxml::xml_node<char>& node, int flags) {
    std::stringstream ss;
    rapidxml::print_to_stream(ss, node, flags);
    return ss.str();
}