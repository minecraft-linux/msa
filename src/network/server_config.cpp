#include <msa/network/server_config.h>

using namespace msa::network;

std::string const ServerConfig::SERVER_DOMAIN = "live.com";
std::string const ServerConfig::LOGIN_BASE_PATH = "https://login." + SERVER_DOMAIN + "/";
std::string ServerConfig::ENDPOINT_DEVICE_ADD = LOGIN_BASE_PATH + "ppsecure/deviceaddcredential.srf";
std::string ServerConfig::ENDPOINT_RST = LOGIN_BASE_PATH + "RST2.srf";
std::string ServerConfig::ENDPOINT_INLINE_CONNECT = LOGIN_BASE_PATH + "ppsecure/InlineConnect.srf?id=80601";
std::string ServerConfig::ENDPOINT_INLINE_CONNECT_PARTNER = LOGIN_BASE_PATH + "ppsecure/InlineConnect.srf?id=80604";