#pragma once

class MSALoginManager;
class MSADeviceAuth;
class MSAAccount;

class MSAStorageManager {

public:

    virtual void readDeviceAuthInfo(MSALoginManager& manager, MSADeviceAuth& deviceAuth) { }
    virtual void onDeviceAuthChanged(MSALoginManager& manager, MSADeviceAuth& deviceAuth) { }
    virtual void onAccountTokenListChanged(MSALoginManager& manager, MSAAccount& account) { }

};