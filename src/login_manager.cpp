#include <msa/login_manager.h>

#include <stdexcept>
#include <msa/network.h>

MSADeviceAuth const& MSALoginManager::requestDeviceAuth() {
    if (!hasReadDeviceAuth && storageManager) {
        storageManager->readDeviceAuthInfo(*this, deviceAuth);
        hasReadDeviceAuth = true;
    }
    if (deviceAuth.membername.empty()) {
        deviceAuth = MSADeviceAuth::generateRandom();
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    if (deviceAuth.puid.empty()) {
        deviceAuth.puid = MSANetwork::addDevice(deviceAuth.membername, deviceAuth.password);
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    if (!deviceAuth.token) {
        deviceAuth.token = MSANetwork::authenticateDevice(deviceAuth.membername, deviceAuth.password);
        if (!deviceAuth.token)
            throw std::runtime_error("Failed to authenticate device");
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    return deviceAuth;
}
