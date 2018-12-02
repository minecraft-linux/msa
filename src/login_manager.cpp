#include <msa/login_manager.h>

#include <stdexcept>
#include <msa/network/device_add_request.h>
#include <msa/network/device_authenticate_request.h>

using namespace msa;

DeviceAuth const& LoginManager::requestDeviceAuth() {
    if (!hasReadDeviceAuth && storageManager) {
        storageManager->readDeviceAuthInfo(deviceAuth);
        hasReadDeviceAuth = true;
    }
    if (deviceAuth.membername.empty()) {
        deviceAuth = DeviceAuth::generateRandom();
        if (storageManager)
            storageManager->saveDeviceAuthInfo(deviceAuth);
    }
    if (deviceAuth.puid.empty()) {
        network::DeviceAddRequest request (deviceAuth.membername, deviceAuth.password);
        auto response = request.send();
        deviceAuth.puid = response.puid;
        if (storageManager)
            storageManager->saveDeviceAuthInfo(deviceAuth);
    }
    if (!deviceAuth.token || deviceAuth.token->isExpired()) {
        network::DeviceAuthenticateRequest request (deviceAuth.membername, deviceAuth.password);
        auto response = request.send();
        deviceAuth.token = response.token;
        if (!deviceAuth.token)
            throw std::runtime_error("Failed to authenticate device");
        if (storageManager)
            storageManager->saveDeviceAuthInfo(deviceAuth);
    }
    return deviceAuth;
}
