#pragma once

#include "security_token_request.h"
#include "../legacy_token.h"

namespace msa {
namespace network {

using AccountTokenResponse = SecurityTokenResponse;

class AccountTokenRequest : public SecurityTokenRequest<AccountTokenResponse> {

protected:
    void buildHeaderAuthInfo(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& authInfo) const override;

    void buildHeaderSecurity(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header) const override;

    void buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body) const override;

    AccountTokenResponse handleResponse(SecurityTokenResponse const& resp) const override {
        return resp;
    }

    std::string generateDeviceProofUri() const;

public:
    std::shared_ptr<LegacyToken> daToken, deviceToken;
    std::vector<SecurityScope> scopes;
    std::string clientAppUri;

    AccountTokenRequest(std::shared_ptr<LegacyToken> daToken, std::shared_ptr<LegacyToken> deviceToken,
                        std::vector<SecurityScope> scopes = {}) : daToken(daToken), deviceToken(deviceToken),
                                                                  scopes(std::move(scopes)) {}

    void addRequestScope(SecurityScope scope) {
        scopes.emplace_back(std::move(scope));
    }

};

}
}