#pragma once

#include <string>
#include <memory>

namespace msa {

class LegacyToken;

class DeviceAuth {

private:
    static std::string generateRandomCredential(const char* allowedChars, int length);

public:
    std::string membername;
    std::string password;
    std::string puid;
    std::shared_ptr<LegacyToken> token;

    DeviceAuth();
    DeviceAuth(std::string membername, std::string password, std::string puid, std::shared_ptr<LegacyToken> token);

    static DeviceAuth generateRandom();

};

}