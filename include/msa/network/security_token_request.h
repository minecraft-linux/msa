#pragma once

#include <msa/token_response.h>
#include <vector>
#include "request.h"
#include "server_config.h"
#include "../scope.h"
#include "xml_sign_context.h"

namespace msa {
namespace network {

struct SecurityTokenResponse;

class SecurityTokenRequestBase : public RequestBase {

protected:
    static const char* const NAMESPACE_SOAP;
    static const char* const NAMESPACE_PPCRL;
    static const char* const NAMESPACE_WSS_SECEXT;
    static const char* const NAMESPACE_SAML;
    static const char* const NAMESPACE_WS_POLICY;
    static const char* const NAMESPACE_WSS_UTILITY;
    static const char* const NAMESPACE_WS_ADDRESSING;
    static const char* const NAMESPACE_WS_SECURECONVERSATION;
    static const char* const NAMESPACE_WS_TRUST;

    static const char* const BINARY_VERSION_STRING;
    static const char* const DEVICE_TYPE;
    static const char* const HOSTING_APP;

    std::string messageId;

    std::string const& getUrl() const override { return ServerConfig::ENDPOINT_RST; }

    void createBody(rapidxml::xml_document<char>& doc) const override;

    virtual LegacyToken* getSigingKey() const { return nullptr; }

    virtual void appendNamespaces(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& envelope) const;

    virtual void buildHeader(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header,
                             XMLSignContext& signContext) const;

    virtual void buildHeaderAuthInfo(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& authInfo) const;

    virtual void buildHeaderSecurity(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header,
                                     XMLSignContext& signContext) const {}

    void buildTimestamp(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& parent,
                        XMLSignContext& signContext) const;

    virtual void buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body,
                           XMLSignContext& signContext) const {}

    void buildTokenRequest(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body,
                           SecurityScope const& scope, int index = 0) const;

    rapidxml::xml_node<char>& buildMultipleTokenRequestElement(rapidxml::xml_document<char>& doc,
                                                               rapidxml::xml_node<char>& body) const;



    SecurityTokenResponse handleResponse(rapidxml::xml_document<char> const& doc) const;

    SecurityTokenResponse sendTokenRequestInternal() const;

private:

    std::string findEncKeyNonce(rapidxml::xml_node<char> const& envelope) const;

public:
    SecurityTokenRequestBase();

};

struct SecurityTokenResponse {

    std::vector<TokenResponse> tokens;

};


template <typename ResponseType>
class SecurityTokenRequest : public SecurityTokenRequestBase {

protected:
    virtual ResponseType handleResponse(SecurityTokenResponse const& resp) const = 0;

public:
    ResponseType send() const {
        return handleResponse(sendTokenRequestInternal());
    }

};

}
}