#pragma once

#include <string>
#include <memory>
class MSALegacyToken;

class MSADeviceAuth {

private:

    static std::string generateRandomCredential(const char* allowedChars, int length);

public:
    std::string membername;
    std::string password;
    std::string puid;
    std::shared_ptr<MSALegacyToken> token;

    MSADeviceAuth();
    MSADeviceAuth(std::string membername, std::string password, std::string puid, std::shared_ptr<MSALegacyToken> token);

    static MSADeviceAuth generateRandom();

};