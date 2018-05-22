#pragma once

#include <string>

namespace msa {
namespace network {

class ServerConfig {

public:
    static std::string const SERVER_DOMAIN;
    static std::string const LOGIN_BASE_PATH;
    static std::string ENDPOINT_DEVICE_ADD;
    static std::string ENDPOINT_RST;
    static std::string ENDPOINT_INLINE_CONNECT;
    static std::string ENDPOINT_INLINE_CONNECT_PARTNER;

};

}
}
