#pragma once

#include <memory>
#include "storage_manager.h"
#include "device_auth.h"

namespace msa {

class StorageManager;

class LoginManager {

private:
    StorageManager* storageManager;
    DeviceAuth deviceAuth;
    bool hasReadDeviceAuth = false;

public:

    LoginManager(StorageManager* storageManager) : storageManager(storageManager) {}

    StorageManager* getStorageManager() const { return storageManager; }

    DeviceAuth const& requestDeviceAuth();

};

}