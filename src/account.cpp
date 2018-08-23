#include <msa/account.h>

#include <msa/login_manager.h>
#include <msa/network/account_token_request.h>
#include <set>

using namespace msa;

Account::Account(std::string username, std::string cid, std::shared_ptr<LegacyToken> daToken,
                 std::shared_ptr<TokenCache> cache)
        : BaseAccountInfo(std::move(username), std::move(cid)), daToken(daToken), tokenCache(cache) {
    //
}


std::unordered_map<SecurityScope, TokenResponse> Account::requestTokens(LoginManager& loginManager,
                                                                        std::vector<SecurityScope> const& scopes,
                                                                        std::string const& clientAppUri) {
    std::vector<SecurityScope> requestScopes;
    std::unordered_map<SecurityScope, TokenResponse> ret;
    std::unordered_map<SecurityScope, std::shared_ptr<Token>> cachedTokens;
    if (tokenCache)
        cachedTokens = tokenCache->getTokensFromCache(*this, scopes);
    for (SecurityScope const& scope : scopes) {
        if (cachedTokens.count(scope) > 0) {
            ret[scope] = TokenResponse(scope, cachedTokens.at(scope));
            continue;
        }
        requestScopes.push_back(scope);
    }
    if (requestScopes.size() == 0)
        return ret;

    network::AccountTokenRequest req(daToken, loginManager.requestDeviceAuth().token, requestScopes);
    req.clientAppUri = clientAppUri;
    auto resp = req.send();
    std::vector<std::shared_ptr<Token>> newTokens;
    if (resp.error) {
        for (SecurityScope const& scope : requestScopes)
            ret[scope] = TokenResponse(scope, resp.error);
        return ret;
    }
    std::set<std::string> scopeAddresses;
    for (SecurityScope const& s : scopes)
        scopeAddresses.insert(s.address);
    for (TokenResponse& token : resp.tokens) {
        if (!token.hasError())
            newTokens.push_back(token.getToken());
        // It seems that the AccountTokenRequest also returns an unasked-for http://Passport.NET/tb token. Therefore we
        // need to filter out tokens we don't need.
        if (scopeAddresses.count(token.getSecurityScope().address) == 0)
            continue;
        ret[token.getSecurityScope()] = token;
    }
    if (newTokens.size() > 0 && tokenCache)
        tokenCache->onTokensReceived(*this, newTokens);
    return ret;
}
