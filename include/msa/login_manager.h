#pragma once

#include <memory>
#include "storage_manager.h"
#include "device_auth.h"

namespace msa {

class StorageManager;

class LoginManager {

private:
    std::shared_ptr<StorageManager> storageManager;
    DeviceAuth deviceAuth;
    bool hasReadDeviceAuth = false;

public:

    LoginManager(std::shared_ptr<StorageManager> storageManager) : storageManager(storageManager) {}

    std::shared_ptr<StorageManager> getStorageManager() const { return storageManager; }

    DeviceAuth const& requestDeviceAuth();

};

}