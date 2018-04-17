#pragma once

#include "security_token_request.h"

namespace msa {
namespace network {

struct DeviceAuthenticateResponse;

class DeviceAuthenticateRequest : public SecurityTokenRequest<DeviceAuthenticateResponse> {

protected:
    void buildHeaderSecurity(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header) const override;

    void buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body) const override;

    DeviceAuthenticateResponse handleResponse(SecurityTokenResponse const& resp) const override;

public:
    std::string username, password;

    DeviceAuthenticateRequest(std::string username, std::string password) : username(username), password(password) {}

};

struct DeviceAuthenticateResponse {

    TokenResponse token;

};

}
}