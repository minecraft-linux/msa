#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "scope.h"

class MSATokenResponse;
class MSALoginManager;
class MSAToken;
class MSALegacyToken;

class MSAAccount {

private:

    std::shared_ptr<MSALoginManager> manager;
    std::string username;
    std::string cid;
    std::shared_ptr<MSALegacyToken> daToken;

    std::unordered_map<MSASecurityScope, std::shared_ptr<MSAToken>> cachedTokens;

public:

    MSAAccount(std::shared_ptr<MSALoginManager> manager, std::string const& username, std::string const& cid,
               std::shared_ptr<MSALegacyToken> daToken);

    std::unordered_map<MSASecurityScope, MSATokenResponse> requestTokens(std::vector<MSASecurityScope> const& scopes);

    std::string const& getUsername() const { return username; }
    std::string const& getCID() const { return cid; }
    std::shared_ptr<MSALegacyToken> getDaToken() const { return daToken; }
    std::unordered_map<MSASecurityScope, std::shared_ptr<MSAToken>> const& getCachedTokens() const { return cachedTokens; }

};