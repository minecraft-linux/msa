#pragma once

#include "token.h"
#include <vector>
#include <unordered_map>

namespace msa {

class Account;

class TokenCache {

public:
    virtual std::shared_ptr<Token> getTokenFromCache(Account& account, std::string const& scopeAddress) = 0;
    virtual void onTokensReceived(Account& account, std::vector<Token*> const& tokens) = 0;

    virtual std::unordered_map<SecurityScope, std::shared_ptr<Token>> getTokensFromCache
            (Account& account, std::vector<SecurityScope> const& scopes) {
        std::unordered_map<SecurityScope, std::shared_ptr<Token>> ret;
        for (SecurityScope const& scope : scopes) {
            auto t = getTokenFromCache(account, scope.address);
            if (t)
                ret.insert({scope, t});
        }
        return ret;
    }

};

}