#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace msa {

class Account;

class AccountManager {

private:
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;

public:
    void addAccount(std::shared_ptr<Account> account);

    void removeAccount(Account& account);

    std::shared_ptr<Account> findAccount(std::string const& cid) const;

};

}