#pragma once

#include <chrono>
#include <vector>
#include "token_response.h"
#include "legacy_token.h"
#include "compact_token.h"

namespace rapidxml { template <typename T> class xml_node; }

namespace msa {

class LegacyNetwork {

private:

    static std::string escapeText(std::string str);

    static std::string escapeURL(std::string str);

    static std::string send(std::string const& url, std::string const& data);

    static std::string generateTimestamp();

    static std::string generateKey(int keyLength, std::string const& sessionKey, std::string const& keyUsage,
                                   std::string const& nonce);

    static std::string generateNonce();

    static std::string generateSignedInfoBlock(std::string const& section, std::string const& text);

    static std::string generateDeviceDAToken(std::shared_ptr<LegacyToken> deviceToken);

    static std::string createSignature(std::string const& data, std::string const& binarySecret,
                                       std::string const& keyUsage, std::string const& nonce);

    static std::string decryptData(rapidxml::xml_node<char>* node, std::string const& binarySecret,
                                   std::string const& nonce);

    static TokenResponse parseTokenResponse(rapidxml::xml_node<char>* node);

    static std::shared_ptr<LegacyToken> parseLegacyToken(rapidxml::xml_node<char>* node, SecurityScope scope,
                                                         Token::TimePoint expire);

    static std::shared_ptr<CompactToken> parseCompactToken(rapidxml::xml_node<char>* node, SecurityScope scope,
                                                           Token::TimePoint expire);

    static std::shared_ptr<TokenErrorInfo> parseErrorInfo(rapidxml::xml_node<char>* node);

public:

    static std::chrono::milliseconds serverTimeOffset;

    static std::chrono::system_clock::time_point getServerTime() {
        return std::chrono::system_clock::now() + serverTimeOffset;
    }

    static std::string addDevice(std::string const& membername, std::string const& password);

    static std::shared_ptr<LegacyToken> authenticateDevice(std::string const& membername, std::string const& password);

    static std::vector<TokenResponse> requestTokens(std::shared_ptr<LegacyToken> daToken,
                                                    std::shared_ptr<LegacyToken> deviceToken,
                                                    std::vector<SecurityScope> const& scopes);

};

}