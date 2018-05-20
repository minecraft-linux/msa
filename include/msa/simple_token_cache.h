#pragma once

#include "token_cache.h"
#include <functional>

namespace msa {

class SimpleTokenCache : public TokenCache {

public:
    using ChangeCallback = std::function<void (Account&)>;

private:
    std::unordered_map<std::string, std::shared_ptr<Token>> cache;
    ChangeCallback cb;

public:
    SimpleTokenCache(ChangeCallback cb) : cb(cb) {}

    void onTokensReceived(Account& account, std::vector<std::shared_ptr<Token>> const& tokens) override;

    std::shared_ptr<Token> getTokenFromCache(Account& account, std::string const& scopeAddress) override;

    std::unordered_map<std::string, std::shared_ptr<Token>> const& getTokens() const {
        return cache;
    }


};

}