#include <msa/login_manager.h>

#include <stdexcept>
#include <msa/legacy_network.h>

using namespace msa;

DeviceAuth const& LoginManager::requestDeviceAuth() {
    if (!hasReadDeviceAuth && storageManager) {
        storageManager->readDeviceAuthInfo(*this, deviceAuth);
        hasReadDeviceAuth = true;
    }
    if (deviceAuth.membername.empty()) {
        deviceAuth = DeviceAuth::generateRandom();
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    if (deviceAuth.puid.empty()) {
        deviceAuth.puid = LegacyNetwork::addDevice(deviceAuth.membername, deviceAuth.password);
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    if (!deviceAuth.token) {
        deviceAuth.token = LegacyNetwork::authenticateDevice(deviceAuth.membername, deviceAuth.password);
        if (!deviceAuth.token)
            throw std::runtime_error("Failed to authenticate device");
        if (storageManager)
            storageManager->onDeviceAuthChanged(*this, deviceAuth);
    }
    return deviceAuth;
}
