#include <msa/network/soap_exception.h>

#include <sstream>
#include <msa/xml_utils.h>

using namespace msa::network;

std::string SoapException::createString() const {
    std::stringstream ss;
    ss << "SOAP error: Code=" << errorCode << ", Subcode=" << errorSubcode << ", Reason=" << errorReason;
    return ss.str();
}

SoapException::SoapException(rapidxml::xml_node<char> const &doc) {
    auto& errorElement = XMLUtils::getRequiredChild(doc, "S:Fault");
    auto& codeElement = XMLUtils::getRequiredChild(errorElement, "S:Code");
    errorCode = XMLUtils::getRequiredChildValue(codeElement, "S:Value");
    auto subcodeElement = codeElement.first_node("S:Subcode");
    if (subcodeElement)
        errorSubcode = XMLUtils::getRequiredChildValue(*subcodeElement, "S:Value");
    auto& reasonElement = XMLUtils::getRequiredChild(errorElement, "S:Reason");
    errorReason = XMLUtils::getRequiredChildValue(reasonElement, "S:Text");
    asString = createString();
}

SoapException SoapException::fromResponse(rapidxml::xml_node<char> const &doc) {
    return SoapException(doc);
}