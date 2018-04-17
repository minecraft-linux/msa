#pragma once

namespace msa {

class LoginManager;
class DeviceAuth;
class Account;

class StorageManager {

public:

    virtual void readDeviceAuthInfo(LoginManager& manager, DeviceAuth& deviceAuth) { }
    virtual void onDeviceAuthChanged(LoginManager& manager, DeviceAuth& deviceAuth) { }
    virtual void onAccountTokenListChanged(LoginManager& manager, Account& account) { }

};

}