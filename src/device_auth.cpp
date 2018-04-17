#include <msa/device_auth.h>

#include <random>
#include <cstring>
#include <msa/legacy_token.h>

using namespace msa;

DeviceAuth::DeviceAuth() {
}

DeviceAuth::DeviceAuth(std::string membername, std::string password, std::string puid,
                       std::shared_ptr<LegacyToken> token) : membername(membername), password(password),
                                                             puid(puid), token(token) {
}

DeviceAuth DeviceAuth::generateRandom() {
    std::string membername = generateRandomCredential("abcdefghijklmnopqrstuvwxyz", 18);
    std::string password = generateRandomCredential("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}/?;:'\\\",.<>`~", 16);
    return DeviceAuth(membername, password, std::string(), std::shared_ptr<LegacyToken>());
}

std::string DeviceAuth::generateRandomCredential(const char* allowedChars, int length) {
    std::string ret;
    ret.resize((size_t) length);
    std::uniform_int_distribution<int> d(0, strlen(allowedChars) - 1);
    std::random_device rd;
    for (int i = 0; i < length; i++)
        ret[i] = allowedChars[d(rd)];
    return ret;
}