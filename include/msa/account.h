#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "scope.h"

namespace msa {

class TokenResponse;
class LoginManager;
class Token;
class LegacyToken;

class Account {

private:

    std::string username;
    std::string cid;
    std::shared_ptr<LegacyToken> daToken;

    std::unordered_map<SecurityScope, std::shared_ptr<Token>> cachedTokens;

public:

    Account(std::string const& username, std::string const& cid, std::shared_ptr<LegacyToken> daToken,
            std::unordered_map<SecurityScope, std::shared_ptr<Token>> cache = {});

    std::unordered_map<SecurityScope, TokenResponse> requestTokens(LoginManager& loginManager,
                                                                   std::vector<SecurityScope> const& scopes);

    std::string const& getUsername() const { return username; }
    std::string const& getCID() const { return cid; }
    std::shared_ptr<LegacyToken> getDaToken() const { return daToken; }
    std::unordered_map<SecurityScope, std::shared_ptr<Token>> const& getCachedTokens() const { return cachedTokens; }

};

}