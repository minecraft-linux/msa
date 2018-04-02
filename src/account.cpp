#include <msa/account.h>

#include <msa/network.h>
#include <msa/login_manager.h>

MSAAccount::MSAAccount(std::shared_ptr<MSALoginManager> manager, std::string const &username, std::string const &cid,
                       std::shared_ptr<MSALegacyToken> daToken) : manager(manager), username(username), cid(cid),
                                                                  daToken(daToken) {
    //
}


std::unordered_map<MSASecurityScope, MSATokenResponse> MSAAccount::requestTokens(std::vector<MSASecurityScope> const& scopes) {
    std::vector<MSASecurityScope> requestScopes;
    std::unordered_map<MSASecurityScope, MSATokenResponse> ret;
    for (MSASecurityScope const& scope : scopes) {
        if (cachedTokens.count(scope) > 0 && !cachedTokens[{scope.address}]->isExpired()) {
            ret[scope] = MSATokenResponse(scope, cachedTokens[{scope.address}]);
            continue;
        }
        requestScopes.push_back(scope);
    }
    std::vector<MSATokenResponse> resp = MSANetwork::requestTokens(daToken, manager->requestDeviceAuth().token, scopes);
    bool hasNewTokens = false;
    for (MSATokenResponse& token : resp) {
        if (!token.hasError()) {
            cachedTokens[token.getSecurityScope()] = token.getToken();
            hasNewTokens = true;
        }
        ret[token.getSecurityScope()] = token;
    }
    if (hasNewTokens && manager->getStorageManager())
        manager->getStorageManager()->onAccountTokenListChanged(*manager, *this);
    return ret;
}
