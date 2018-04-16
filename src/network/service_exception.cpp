#include <msa/network/service_exception.h>
#include <msa/network/xml_utils.h>
#include <sstream>

using namespace msa::network;

std::string ServiceException::createString() const {
    std::stringstream ss;
    ss << "Service error: Code=0x" << std::hex << errorCode << ", Subcode=0x"
       << std::hex << (unsigned int) errorSubcode;
    return ss.str();
}

ServiceException ServiceException::fromResponse(rapidxml::xml_node<char> const& doc) {
    auto& errorElement = XMLUtils::getRequiredChild(doc, "Error");
    int errorCode = (int) std::strtol(XMLUtils::getAttribute(errorElement, "Code"), nullptr, 16);
    int errorSubcode = (int) std::strtoll(XMLUtils::getRequiredChild(doc, "ErrorSubcode").value(), nullptr, 16);
    return ServiceException(errorCode, errorSubcode);
}
