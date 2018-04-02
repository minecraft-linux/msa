#pragma once

#include <string>
#include <memory>
#include "token.h"

class MSAErrorInfo;

class MSATokenResponse {

private:
    MSASecurityScope securityScope;
    std::shared_ptr<MSAToken> token;
    std::shared_ptr<MSAErrorInfo> error;

public:
    MSATokenResponse() { }
    MSATokenResponse(MSASecurityScope scope, std::shared_ptr<MSAToken> token) : securityScope(scope), token(token) { }
    MSATokenResponse(MSASecurityScope scope, std::shared_ptr<MSAErrorInfo> error) : securityScope(scope), error(error) { }

    bool hasError() const { return token == nullptr; }
    MSASecurityScope const& getSecurityScope() const { return securityScope; }
    std::shared_ptr<MSAToken> getToken() { return token; }
    std::shared_ptr<MSAErrorInfo> getError() { return error; }

};

struct MSAErrorInfo {
    unsigned int reqStatus = 0;
    unsigned int errorStatus = 0;
    std::string flowUrl;
    std::string inlineAuthUrl;
    std::string inlineEndAuthUrl;
};