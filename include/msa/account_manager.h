#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace msa {

class Account;
class StorageManager;
class LegacyToken;
class BaseAccountInfo;

struct NoSuchAccountException : public std::exception {
    virtual const char* what() const noexcept { return "No account found"; }
};
struct AccountAlreadyExistsException : public std::exception {
    virtual const char* what() const noexcept { return "Account with this CID already exists"; }
};

class AccountManager {

private:
    StorageManager& storageManager;
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;

    void addAccount(std::shared_ptr<Account> account);

public:
    AccountManager(StorageManager& storageManager) : storageManager(storageManager) {}

    std::vector<BaseAccountInfo> getAccounts();

    std::shared_ptr<Account> addAccount(std::string username, std::string cid, std::shared_ptr<LegacyToken> daToken);

    void removeAccount(Account& account);

    std::shared_ptr<Account> findAccount(std::string const& cid);

};

}