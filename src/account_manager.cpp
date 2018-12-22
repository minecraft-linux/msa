#include <memory>

#include <msa/account_manager.h>
#include <msa/account.h>
#include <msa/storage_manager.h>

using namespace msa;

AccountManager::AccountManager(msa::StorageManager &storageManager) : storageManager(storageManager) {
    changeCallback = std::make_shared<Account::ChangeCallback>([this](Account& account) {
        this->storageManager.saveAccount(account);
    });
}

AccountManager::~AccountManager() {
    std::lock_guard<std::recursive_mutex> lock (accountsMutex);
    for (auto const& p : accounts)
        p.second->removeChangeCallback(changeCallback);
}

std::shared_ptr<Account> AccountManager::addAccount(std::string username, std::string cid, std::string puid,
                                                    std::shared_ptr<LegacyToken> daToken) {
    std::lock_guard<std::recursive_mutex> lock (accountsMutex);
    if (accounts.count(cid) > 0)
        throw AccountAlreadyExistsException();
    auto tokenCache = storageManager.createTokenCache(cid);
    std::shared_ptr<Account> account(new Account(std::move(username), std::move(cid), std::move(puid),
            std::move(daToken), tokenCache));
    addAccount(account);
    storageManager.saveAccount(*account);
    return account;
}

std::vector<BaseAccountInfo> AccountManager::getAccounts() {
    return storageManager.getAccounts();
}

void AccountManager::addAccount(std::shared_ptr<Account> account) {
    std::lock_guard<std::recursive_mutex> lock (accountsMutex);
    account->addChangeCallback(changeCallback);
    if (!accounts.insert({account->getCID(), account}).second)
        throw AccountAlreadyExistsException();
}

void AccountManager::removeAccount(Account& account) {
    std::lock_guard<std::recursive_mutex> lock (accountsMutex);
    auto it = accounts.find(account.getCID());
    if (it == accounts.end())
        throw NoSuchAccountException();
    if (it->second.get() != &account)
        throw std::runtime_error("An account with a CID of the same value as the specified account does exist, "
                                         "but is not the specified instance");
    storageManager.removeAccount(account);
    accounts.erase(it);
}

std::shared_ptr<Account> AccountManager::findAccount(std::string const& cid) {
    std::lock_guard<std::recursive_mutex> lock (accountsMutex);
    auto it = accounts.find(cid);
    if (it != accounts.end())
        return it->second;
    try {
        auto account = storageManager.readAccount(cid);
        if (account) {
            addAccount(account);
            return account;
        }
    } catch (std::exception& e) {
    }
    return std::shared_ptr<Account>();
}