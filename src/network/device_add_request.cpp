#include <stdexcept>
#include <cstring>
#include <msa/network/device_add_request.h>
#include <rapidxml.hpp>
#include <msa/network/parse_exception.h>
#include <msa/network/service_exception.h>
#include <msa/network/xml_utils.h>

using namespace rapidxml;
using namespace msa::network;

void DeviceAddRequest::createBody(rapidxml::xml_document<char>& doc) const {
    auto root = doc.allocate_node(node_element, "DeviceAddRequest", "");
    doc.append_node(root);

    auto clientInfo = doc.allocate_node(node_element, "ClientInfo", "");
    clientInfo->append_attribute(doc.allocate_attribute("name", "MSAAndroidApp"));
    clientInfo->append_attribute(doc.allocate_attribute("version", "1.0"));
    root->append_node(clientInfo);

    auto authentication = doc.allocate_node(node_element, "Authentication", "");
    authentication->append_node(doc.allocate_node(node_element, "Membername", membername.c_str(), 0, membername.length()));
    authentication->append_node(doc.allocate_node(node_element, "Password", password.c_str(), 0, password.length()));
    root->append_node(authentication);
}

DeviceAddResponse DeviceAddRequest::handleResponse(rapidxml::xml_document<char> const& doc) const {
    auto& root = XMLUtils::getRequiredChild(doc, "DeviceAddResponse");
    if (strcmp(XMLUtils::getAttribute(root, "Success"), "true") != 0)
        throw ServiceException::fromResponse(root);
    DeviceAddResponse ret;
    ret.puid = XMLUtils::getRequiredChild(root, "puid").value();
    return ret;
}