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
#include <msa/simple_token_cache.h>

using namespace msa;
using namespace rapidxml;

SimpleStorageManager::SimpleStorageManager(std::string const& basePath) {
    if (basePath.length() == 0 || basePath[basePath.length() - 1] == '/')
        this->basePath = basePath;
    else
        this->basePath = basePath + "/";

    mkdir((this->basePath + "accounts/").c_str(), 0700);
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
    data.resize((size_t) fs.tellg() + 1);

    fs.seekg(0, std::ios_base::beg);
    fs.read(&data[0], data.size() - 1);
    data[data.size() - 1] = '\0';

    return data;
}

std::vector<BaseAccountInfo> SimpleStorageManager::getAccounts() {
    std::string accountsPath = this->basePath + "accounts/";
    mkdir(accountsPath.c_str(), 0700);
    DIR* dir = opendir(accountsPath.c_str());
    struct dirent* de;
    std::vector<BaseAccountInfo> ret;
    while ((de = readdir(dir)) != nullptr) {
        size_t d_name_len = strlen(de->d_name);
        if (d_name_len < 8 + 4 || memcmp("account_", de->d_name, 8) || memcmp(".xml", de->d_name + d_name_len - 4, 4))
            continue;
        auto account = readAccountFile(accountsPath + de->d_name);
        ret.push_back(BaseAccountInfo(account->getUsername(), account->getCID()));
    }
    closedir(dir);
    return ret;
}

void SimpleStorageManager::readDeviceAuthInfo(DeviceAuth& deviceAuth) {
    std::ifstream fs (getDeviceAuthInfoPath());
    if (!fs)
        return;
    auto fd = readFile(fs);
    fs.close();
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

void SimpleStorageManager::saveDeviceAuthInfo(DeviceAuth& deviceAuth) {
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

std::shared_ptr<TokenCache> SimpleStorageManager::createTokenCache(
        std::string const& cid, std::unordered_map<std::string, std::shared_ptr<Token>> cache) {
    return std::shared_ptr<TokenCache>(new SimpleTokenCache(
            std::bind(&SimpleStorageManager::saveAccount, this, std::placeholders::_1), cache));
}

std::shared_ptr<TokenCache> SimpleStorageManager::createTokenCache(std::string const& cid) {
    return createTokenCache(cid, {});
}

std::shared_ptr<Account> SimpleStorageManager::readAccountFile(std::string const& path) {
    std::ifstream fs (path);
    if (!fs)
        throw std::runtime_error("Failed to open account file for reading");
    auto fd = readFile(fs);
    fs.close();
    rapidxml::xml_document<char> doc;
    doc.parse<0>(fd.data());

    auto& root = XMLUtils::getRequiredChild(doc, "MsaAccount");
    if (strcmp(XMLUtils::getAttribute(root, "version", ""), "1"))
        throw std::runtime_error("Invalid version");
    std::string cid = XMLUtils::getRequiredChildValue(root, "CID");
    auto puidNode = root.first_node("PUID");
    std::string puid = puidNode ? XMLUtils::getValue(*puidNode) : "";
    std::string username = XMLUtils::getRequiredChildValue(root, "Username");
    auto daTokenNode = root.first_node("DaToken");
    std::shared_ptr<LegacyToken> daToken;
    if (daTokenNode)
        daToken = token_pointer_cast<LegacyToken>(Token::fromXml(*daTokenNode));
    std::unordered_map<std::string, std::shared_ptr<Token>> cache;
    for (auto it = root.first_node("CachedToken"); it != nullptr; it = it->next_sibling("CachedToken")) {
        auto token = Token::fromXml(*it);
        cache.insert({token->getSecurityScope().address, token});
    }
    return std::shared_ptr<Account>(new Account(username, cid, puid, daToken, createTokenCache(cid, std::move(cache))));
}

std::shared_ptr<Account> SimpleStorageManager::readAccount(std::string const& cid) {
    return readAccountFile(getAccountPath(cid));
}

void SimpleStorageManager::saveAccount(Account const& account) {
    rapidxml::xml_document<char> doc;
    auto root = doc.allocate_node(node_element, "MsaAccount");
    doc.append_node(root);
    root->append_attribute(doc.allocate_attribute("version", "1"));
    root->append_node(doc.allocate_node(node_element, "CID", account.getCID().c_str()));
    root->append_node(doc.allocate_node(node_element, "PUID", account.getPUID().c_str()));
    root->append_node(doc.allocate_node(node_element, "Username", account.getUsername().c_str()));
    auto daToken = account.getDaToken();
    if (daToken) {
        auto tokenNode = doc.allocate_node(node_element, "DaToken");
        root->append_node(tokenNode);
        daToken->toXml(*tokenNode);
    }
    auto& cachedTokens = std::dynamic_pointer_cast<SimpleTokenCache>(account.getCache())->getTokens();
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

void SimpleStorageManager::removeAccount(Account& account) {
    remove(getAccountPath(account).c_str());
}
