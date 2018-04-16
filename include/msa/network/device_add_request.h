#pragma once

#include "request.h"
#include "server_config.h"

namespace msa {
namespace network {

struct DeviceAddResponse;

class DeviceAddRequest : public Request<DeviceAddResponse> {

private:
    std::string const& getUrl() const override { return ServerConfig::ENDPOINT_DEVICE_ADD; }

    void createBody(rapidxml::xml_document<char>& doc) const override;

    DeviceAddResponse handleResponse(rapidxml::xml_document<char> const& doc) const override;

public:
    std::string membername, password;

    DeviceAddRequest(std::string membername, std::string password) : membername(membername), password(password) {}

};

struct DeviceAddResponse {

    std::string puid;

};

}
}