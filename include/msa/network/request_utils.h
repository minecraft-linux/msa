#pragma once

#include <string>
#include <vector>

namespace msa {
namespace network {

class RequestUtils {

public:
    static std::string createNonce(size_t size = 32);

    static inline bool isUrlSafe(char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                (c == '-' || c == '_' || c == '.' || c == '~');
    }

    static std::string urlEscape(std::string const& str);

    static std::string encodeUrlParams(std::vector<std::pair<std::string, std::string>> const& data);

};

}
}