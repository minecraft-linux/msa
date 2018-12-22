#pragma once

#include <curl/curl.h>
#include <functional>

namespace msa {
namespace network {

class BaseRequest;

class CURLManager {

private:
    friend class RequestBase;

    static std::function<void (CURL* curl)> initHook;

public:
    static void setCurlInitHook(std::function<void (CURL* curl)> func);

};

}
}