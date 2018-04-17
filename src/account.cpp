#include <msa/account.h>

#include <msa/legacy_network.h>
#include <msa/login_manager.h>

using namespace msa;

Account::Account(std::shared_ptr<LoginManager> manager, std::string const &username, std::string const &cid,
                 std::shared_ptr<LegacyToken> daToken) : manager(manager), username(username), cid(cid), 
                                                         daToken(daToken) {
    //
}


std::unordered_map<SecurityScope, TokenResponse> Account::requestTokens(std::vector<SecurityScope> const& scopes) {
    std::vector<SecurityScope> requestScopes;
    std::unordered_map<SecurityScope, TokenResponse> ret;
    for (SecurityScope const& scope : scopes) {
        if (cachedTokens.count(scope) > 0 && !cachedTokens[{scope.address}]->isExpired()) {
            ret[scope] = TokenResponse(scope, cachedTokens[{scope.address}]);
            continue;
        }
        requestScopes.push_back(scope);
    }
    std::vector<TokenResponse> resp = LegacyNetwork::requestTokens(daToken, manager->requestDeviceAuth().token, scopes);
    bool hasNewTokens = false;
    for (TokenResponse& token : resp) {
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
