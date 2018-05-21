#pragma once

#include "storage_manager.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace msa {

class SimpleStorageManager : public StorageManager {

private:
    std::string basePath;

    std::string getDeviceAuthInfoPath() const;
    std::string getAccountPath(std::string const& cid) const;
    std::string getAccountPath(Account const& account) const;

    static std::vector<char> readFile(std::ifstream& fs);

public:
    explicit SimpleStorageManager(std::string const& basePath);

    std::vector<BaseAccountInfo> getAccounts() override;

    void readDeviceAuthInfo(DeviceAuth& deviceAuth) override;
    void saveDeviceAuthInfo(DeviceAuth& deviceAuth) override;

    std::shared_ptr<TokenCache> createTokenCache(std::string const& cid) override;
    std::shared_ptr<Account> readAccount(std::string const& cid) override;
    void saveAccount(Account const& account) override;
    void removeAccount(Account& account) override;

};

}