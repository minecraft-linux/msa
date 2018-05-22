#include <msa/simple_token_cache.h>

using namespace msa;

void SimpleTokenCache::onTokensReceived(Account& account, std::vector<std::shared_ptr<Token>> const& tokens) {
    for (auto const& token : tokens)
        cache[token->getSecurityScope().address] = token;
    if (cb)
        cb(account);
}

std::shared_ptr<Token> SimpleTokenCache::getTokenFromCache(Account& account, std::string const& scopeAddress) {
    auto it = cache.find(scopeAddress);
    if (it == cache.end())
        return std::shared_ptr<Token>();
    if (it->second->isExpired()) {
        cache.erase(it);
        cb(account);
        return std::shared_ptr<Token>();
    }
    return it->second;
}