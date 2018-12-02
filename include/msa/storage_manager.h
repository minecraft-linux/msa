#pragma once

#include <memory>
#include <vector>
#include "account.h"

namespace msa {

class LoginManager;
class DeviceAuth;
class Account;
class BaseAccountInfo;

class AccountStorageManager {

public:
    virtual void saveAccount(Account const& account) = 0;

};

class StorageManager {

public:
    virtual ~StorageManager() {}

    virtual void readDeviceAuthInfo(DeviceAuth& deviceAuth) {}
    virtual void saveDeviceAuthInfo(DeviceAuth& deviceAuth) {}

    virtual std::vector<BaseAccountInfo> getAccounts() { return {}; }
    virtual std::shared_ptr<TokenCache> createTokenCache(std::string const& cid) { return std::shared_ptr<TokenCache>(); }
    virtual std::shared_ptr<Account> readAccount(std::string const& cid) { return std::shared_ptr<Account>(); }
    virtual void saveAccount(Account const& account) {}
    virtual void removeAccount(Account& account) {}

};

}