#pragma once

#include "storage_manager.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace msa {

class SimpleStorageManager : public StorageManager {

private:
    std::string basePath;
    std::unordered_map<Account*, std::shared_ptr<Account>> accounts;

    std::string getDeviceAuthInfoPath() const;
    std::string getAccountPath(std::string const& cid) const;
    std::string getAccountPath(Account const& account) const;

    static std::vector<char> readFile(std::ifstream& fs);

    std::shared_ptr<Account> readAccountInfo(std::shared_ptr<LoginManager> mgr, std::string const& path);

    void saveAccountInfo(Account const& account);

public:
    SimpleStorageManager(std::string const& basePath);

    void addAccount(std::shared_ptr<Account> account);
    void removeAccount(std::shared_ptr<Account> account);

    void readDeviceAuthInfo(LoginManager&, DeviceAuth& deviceAuth) override;
    void onDeviceAuthChanged(LoginManager&, DeviceAuth& deviceAuth) override;
    void onAccountTokenListChanged(LoginManager& manager, Account& account) override;

};

}