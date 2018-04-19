#pragma once

#include <string>
#include <vector>

namespace msa {
namespace network {

class CryptoUtils {

public:
    static std::string createNonce(size_t size = 32);

    static std::string sign(std::string const& data, std::string const& binarySecret,
                            std::string const& keyUsage, std::string const& nonce);

    static std::string generateSharedKey(size_t keyLength, std::string const& sessionKey, std::string const& keyUsage,
                                         std::string const& nonce);

};

}
}