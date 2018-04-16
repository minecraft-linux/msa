#pragma once

#include <string>
#include <rapidxml.hpp>

namespace msa {
namespace network {

class RequestBase {

protected:
    virtual std::string const& getUrl() const = 0;

    virtual void createBody(rapidxml::xml_document<char>& doc) const = 0;

    std::string sendInternal() const;

};

template <typename ResponseType>
class Request : public RequestBase {

protected:
    virtual ResponseType handleResponse(rapidxml::xml_document<char> const& doc) const = 0;

public:
    ResponseType send() const {
        rapidxml::xml_document<char> doc;
        std::string str = sendInternal();
        doc.parse<0>(&str[0]);
        return handleResponse(doc);
    }

};

}
}