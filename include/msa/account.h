#pragma once

#include <string>
#include <set>
#include <memory>
#include <unordered_map>
#include <functional>
#include "scope.h"
#include "token_cache.h"

namespace msa {

class TokenResponse;
class LoginManager;
class Token;
class LegacyToken;

struct BaseAccountInfo {

protected:

    std::string username;
    std::string cid;

public:

    BaseAccountInfo(std::string username, std::string cid) : username(std::move(username)), cid(std::move(cid)) {}

    std::string const& getUsername() const { return username; }
    std::string const& getCID() const { return cid; }

};

class Account : public BaseAccountInfo {

public:

    using ChangeCallback = std::function<void (Account&)>;

protected:

    std::string puid;
    std::shared_ptr<LegacyToken> daToken;
    std::shared_ptr<TokenCache> tokenCache;
    std::set<std::shared_ptr<ChangeCallback>> changeCallbacks;

public:

    Account(std::string username, std::string puid, std::string cid, std::shared_ptr<LegacyToken> daToken,
            std::shared_ptr<TokenCache> cache);

    void addChangeCallback(std::shared_ptr<ChangeCallback> callback);

    void removeChangeCallback(std::shared_ptr<ChangeCallback> callback);

    void updateDetails(std::string username, std::shared_ptr<LegacyToken> daToken);

    std::unordered_map<SecurityScope, TokenResponse> requestTokens(LoginManager& loginManager,
                                                                   std::vector<SecurityScope> const& scopes,
                                                                   std::string const& clientAppUri = std::string());

    std::string const& getPUID() const { return puid; }

    std::shared_ptr<LegacyToken> getDaToken() const { return daToken; }

    std::shared_ptr<TokenCache> getCache() const { return tokenCache; }

};

}