#include <msa/network/server_config.h>

using namespace msa::network;

std::string const ServerConfig::SERVER_DOMAIN = "live.com";
std::string const ServerConfig::LOGIN_BASE_PATH = "https://login." + SERVER_DOMAIN + "/";
std::string ServerConfig::ENDPOINT_DEVICE_ADD = LOGIN_BASE_PATH + "ppsecure/deviceaddcredential.srf";
std::string ServerConfig::ENDPOINT_RST = LOGIN_BASE_PATH + "RST2.srf";