#include <msa/account.h>

#include <msa/legacy_network.h>
#include <msa/login_manager.h>

using namespace msa;

Account::Account(std::string username, std::string cid, std::shared_ptr<LegacyToken> daToken,
                 std::shared_ptr<TokenCache> cache)
        : BaseAccountInfo(std::move(username), std::move(cid)), daToken(daToken), tokenCache(cache) {
    //
}


std::unordered_map<SecurityScope, TokenResponse> Account::requestTokens(LoginManager& loginManager,
                                                                        std::vector<SecurityScope> const& scopes) {
    std::vector<SecurityScope> requestScopes;
    std::unordered_map<SecurityScope, TokenResponse> ret;
    std::unordered_map<SecurityScope, std::shared_ptr<Token>> cachedTokens;
    if (tokenCache)
        tokenCache->getTokensFromCache(*this, scopes);
    for (SecurityScope const& scope : scopes) {
        if (cachedTokens.count(scope) > 0) {
            ret[scope] = TokenResponse(scope, cachedTokens[{scope.address}]);
            continue;
        }
        requestScopes.push_back(scope);
    }
    std::vector<TokenResponse> resp = LegacyNetwork::requestTokens(daToken, loginManager.requestDeviceAuth().token, scopes);
    std::vector<std::shared_ptr<Token>> newTokens;
    for (TokenResponse& token : resp) {
        if (!token.hasError()) {
            newTokens.push_back(token.getToken());
        }
        ret[token.getSecurityScope()] = token;
    }
    if (newTokens.size() > 0 && tokenCache)
        tokenCache->onTokensReceived(*this, newTokens);
    return ret;
}
