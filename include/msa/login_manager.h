#pragma once

#include <memory>
#include "storage_manager.h"
#include "device_auth.h"

class MSAStorageManager;

class MSALoginManager {

private:
    std::shared_ptr<MSAStorageManager> storageManager;
    MSADeviceAuth deviceAuth;
    bool hasReadDeviceAuth = false;

public:

    MSALoginManager(std::shared_ptr<MSAStorageManager> storageManager) : storageManager(storageManager) { }

    std::shared_ptr<MSAStorageManager> getStorageManager() const { return storageManager; }

    MSADeviceAuth const& requestDeviceAuth();

};

