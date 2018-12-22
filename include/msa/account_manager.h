#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "account.h"

namespace msa {

class Account;
class StorageManager;
class LegacyToken;
class BaseAccountInfo;

struct NoSuchAccountException : public std::exception {
    const char* what() const noexcept override { return "No account found"; }
};
struct AccountAlreadyExistsException : public std::exception {
    const char* what() const noexcept override { return "Account with this CID already exists"; }
};

class AccountManager {

private:
    StorageManager& storageManager;
    std::mutex accountsMutex;
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;
    std::shared_ptr<Account::ChangeCallback> changeCallback;

    void addAccount(std::shared_ptr<Account> account);

public:
    explicit AccountManager(StorageManager& storageManager);

    ~AccountManager();

    std::vector<BaseAccountInfo> getAccounts();

    std::shared_ptr<Account> addAccount(std::string username, std::string cid, std::string puid,
            std::shared_ptr<LegacyToken> daToken);

    void removeAccount(Account& account);

    std::shared_ptr<Account> findAccount(std::string const& cid);

};

}