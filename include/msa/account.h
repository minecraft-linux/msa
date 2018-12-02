#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "scope.h"
#include "token_cache.h"

namespace msa {

class TokenResponse;
class LoginManager;
class Token;
class LegacyToken;

struct BaseAccountInfo {

private:

    std::string username;
    std::string cid;

protected:

    void setUsername(std::string const& username) { this->username = username; }

public:

    BaseAccountInfo(std::string username, std::string cid) : username(std::move(username)), cid(std::move(cid)) {}

    std::string const& getUsername() const { return username; }
    std::string const& getCID() const { return cid; }

};

class Account : public BaseAccountInfo {

private:

    std::shared_ptr<LegacyToken> daToken;
    std::shared_ptr<TokenCache> tokenCache;

protected:

    friend class AccountManager;

    void setDaToken(std::shared_ptr<LegacyToken> daToken) { this->daToken = std::move(daToken); }

public:

    Account(std::string username, std::string cid, std::shared_ptr<LegacyToken> daToken,
            std::shared_ptr<TokenCache> cache);

    std::unordered_map<SecurityScope, TokenResponse> requestTokens(LoginManager& loginManager,
                                                                   std::vector<SecurityScope> const& scopes,
                                                                   std::string const& clientAppUri = std::string());

    std::shared_ptr<LegacyToken> getDaToken() const { return daToken; }

    std::shared_ptr<TokenCache> getCache() const { return tokenCache; }

};

}