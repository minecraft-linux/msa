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
#include <dirent.h>

using namespace msa;
using namespace rapidxml;

SimpleStorageManager::SimpleStorageManager(std::string const& basePath) {
    if (basePath.length() > 0 || basePath[basePath.length() - 1] == '/')
        this->basePath = basePath;
    else
        this->basePath = basePath + "/";

    std::string accountsPath = this->basePath + "accounts/";
    mkdir(accountsPath.c_str(), 0700);
    DIR* dir = opendir(accountsPath.c_str());
    struct dirent* de;
    while ((de = readdir(dir)) != nullptr) {
        size_t d_name_len = strlen(de->d_name);
        if (d_name_len < 8 + 4 || memcmp("account_", de->d_name, 8) || memcmp(".xml", de->d_name + d_name_len - 4, 4))
            continue;
        auto account = readAccountInfo(accountsPath + de->d_name);
        accounts[account.get()] = account;
    }
    closedir(dir);
}

std::string SimpleStorageManager::getDeviceAuthInfoPath() const {
    return basePath + "deviceAuth.xml";
}

std::string SimpleStorageManager::getAccountPath(std::string const& cid) const {
    return basePath + "accounts/account_" + cid + ".xml";
}

std::string SimpleStorageManager::getAccountPath(Account const& account) const {
    return getAccountPath(account.getCID());
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

std::shared_ptr<Account> SimpleStorageManager::readAccountInfo(std::string const& path) {
    std::ifstream fs (path);
    if (!fs)
        throw std::runtime_error("Failed to open account file for reading");
    auto fd = readFile(fs);
    rapidxml::xml_document<char> doc;
    doc.parse<0>(fd.data());

    auto& root = XMLUtils::getRequiredChild(doc, "MsaAccount");
    if (strcmp(XMLUtils::getAttribute(root, "version", ""), "1"))
        throw std::runtime_error("Invalid version");
    std::string cid = XMLUtils::getRequiredChildValue(root, "CID");
    std::string username = XMLUtils::getRequiredChildValue(root, "Username");
    auto daTokenNode = root.first_node("Token");
    std::shared_ptr<LegacyToken> daToken;
    if (daTokenNode)
        daToken = token_pointer_cast<LegacyToken>(Token::fromXml(*daTokenNode));
    std::unordered_map<SecurityScope, std::shared_ptr<Token>> cache;
    for (auto it = root.first_node("CachedToken"); it != nullptr; it = it->next_sibling("CachedToken")) {
        auto token = Token::fromXml(*it);
        cache.insert({token->getSecurityScope(), token});
    }
    return std::shared_ptr<Account>(new Account(username, cid, daToken, cache));
}

void SimpleStorageManager::saveAccountInfo(Account const& account) {
    rapidxml::xml_document<char> doc;
    auto root = doc.allocate_node(node_element, "MsaAccount");
    doc.append_node(root);
    root->append_attribute(doc.allocate_attribute("version", "1"));
    root->append_node(doc.allocate_node(node_element, "CID", account.getCID().c_str()));
    root->append_node(doc.allocate_node(node_element, "Username", account.getUsername().c_str()));
    auto daToken = account.getDaToken();
    if (daToken) {
        auto tokenNode = doc.allocate_node(node_element, "DaToken");
        root->append_node(tokenNode);
        daToken->toXml(*tokenNode);
    }
    auto& cachedTokens = account.getCachedTokens();
    for (auto const& t : cachedTokens) {
        auto tokenNode = doc.allocate_node(node_element, "CachedToken");
        root->append_node(tokenNode);
        t.second->toXml(*tokenNode);
    }

    std::ofstream fs(getAccountPath(account));
    if (!fs)
        throw std::runtime_error("Failed to open device auth info for writing");
    rapidxml::print_to_stream(fs, doc, rapidxml::print_no_indenting);
}

void SimpleStorageManager::addAccount(std::shared_ptr<Account> account) {
    accounts.insert({account.get(), account});
    saveAccountInfo(*account);
}

void SimpleStorageManager::removeAccount(std::shared_ptr<Account> account) {
    accounts.erase(account.get());
    remove(getAccountPath(*account).c_str());
}

void SimpleStorageManager::onAccountTokenListChanged(LoginManager& manager, Account& account) {
    if (accounts.count(&account) > 0)
        saveAccountInfo(account);
}