#include <msa/account_manager.h>
#include <msa/account.h>

using namespace msa;

void AccountManager::addAccount(std::shared_ptr<Account> account) {
    if (!accounts.insert({account->getCID(), account}).second)
        throw std::runtime_error("Account with this CID ia already added");
}

void AccountManager::removeAccount(Account& account) {
    auto it = accounts.find(account.getCID());
    if (it == accounts.end())
        throw std::runtime_error("No such account found to delete");
    if (it->second.get() != &account)
        throw std::runtime_error("An account with a CID of the same value as the specified account does exist, "
                                         "but is not the specified instance");
    accounts.erase(it);
}

std::shared_ptr<Account> AccountManager::findAccount(std::string const& cid) const {
    auto it = accounts.find(cid);
    if (it == accounts.end())
        return std::shared_ptr<Account>();
    return it->second;
}