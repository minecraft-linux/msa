#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace msa {

class Account;
class StorageManager;

class AccountManager {

private:
    StorageManager& storageManager;
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;

public:
    AccountManager(StorageManager& storageManager) : storageManager(storageManager) {}

    void addAccount(std::shared_ptr<Account> account);

    void removeAccount(Account& account);

    std::shared_ptr<Account> findAccount(std::string const& cid);

};

}