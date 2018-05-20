#include <msa/simple_storage_manager.h>
#include <msa/account.h>
#include <sys/stat.h>
#include <rapidxml.hpp>
#include <msa/device_auth.h>
#include <msa/legacy_token.h>
#include <fstream>
#include <rapidxml_print.hpp>
#include <msa/xml_utils.h>
#include <cstring>

using namespace msa;
using namespace rapidxml;

SimpleStorageManager::SimpleStorageManager(std::string const& basePath) {
    if (basePath.length() > 0 || basePath[basePath.length() - 1] == '/')
        this->basePath = basePath;
    else
        this->basePath = basePath + "/";
    mkdir((this->basePath + "accounts/").c_str(), 0700);
}

std::string SimpleStorageManager::getDeviceAuthInfoPath() const {
    return basePath + "deviceAuth.xml";
}

std::string SimpleStorageManager::getAccountPath(Account const& account) const {
    return basePath + "accounts/account_" + account.getCID() + ".xml";
}

std::vector<char> SimpleStorageManager::readFile(std::ifstream& fs) {
    std::vector<char> data;

    fs.seekg(0, std::ios_base::end);
    data.resize((size_t) fs.tellg());

    fs.seekg(0, std::ios_base::beg);
    fs.read(&data[0], data.size());

    return data;
}

void SimpleStorageManager::readDeviceAuthInfo(LoginManager&, DeviceAuth& deviceAuth) {
    std::ifstream fs (getDeviceAuthInfoPath());
    if (!fs)
        return;
    auto fd = readFile(fs);
    rapidxml::xml_document<char> doc;
    doc.parse<0>(fd.data());

    auto& root = XMLUtils::getRequiredChild(doc, "MsaDeviceAuthInfo");
    if (strcmp(XMLUtils::getAttribute(root, "version", ""), "1"))
        throw std::runtime_error("Invalid version");
    deviceAuth.puid = XMLUtils::getRequiredChildValue(root, "PUID");
    deviceAuth.membername = XMLUtils::getRequiredChildValue(root, "Membername");
    deviceAuth.password = XMLUtils::getRequiredChildValue(root, "Password");
    auto token = root.first_node("Token");
    if (token)
        deviceAuth.token = token_pointer_cast<LegacyToken>(Token::fromXml(*token));
}

void SimpleStorageManager::onDeviceAuthChanged(LoginManager&, DeviceAuth& deviceAuth) {
    rapidxml::xml_document<char> doc;
    auto root = doc.allocate_node(node_element, "MsaDeviceAuthInfo");
    doc.append_node(root);
    root->append_attribute(doc.allocate_attribute("version", "1"));
    root->append_node(doc.allocate_node(node_element, "PUID", deviceAuth.puid.c_str()));
    root->append_node(doc.allocate_node(node_element, "Membername", deviceAuth.membername.c_str()));
    root->append_node(doc.allocate_node(node_element, "Password", deviceAuth.password.c_str()));
    if (deviceAuth.token) {
        auto tokenNode = doc.allocate_node(node_element, "Token");
        root->append_node(tokenNode);
        deviceAuth.token->toXml(*tokenNode);
    }

    std::ofstream fs(getDeviceAuthInfoPath());
    if (!fs)
        throw std::runtime_error("Failed to open device auth info for writing");
    rapidxml::print_to_stream(fs, doc, rapidxml::print_no_indenting);
}

void SimpleStorageManager::addAccount(std::shared_ptr<Account> account) {
    accounts.insert({account.get(), account});
}

void SimpleStorageManager::removeAccount(std::shared_ptr<Account> account) {
    accounts.erase(account.get());
}

void SimpleStorageManager::onAccountTokenListChanged(LoginManager& manager, Account& account) {
}