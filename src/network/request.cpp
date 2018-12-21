#include <msa/network/request.h>
#include <msa/network/curl.h>

#include <log.h>
#include <sstream>
#include <curl/curl.h>
#include <rapidxml_print.hpp>
#include <cstring>
#include <msa/network/server_time.h>

using namespace msa::network;

std::function<void (CURL* curl)> CURLManager::initHook;

void CURLManager::setCurlInitHook(std::function<void (CURL* curl)> func) {
    initHook = func;
}

static size_t curl_stringstream_write_func(void* ptr, size_t size, size_t nmemb, std::stringstream* s) {
    s->write((char*) ptr, size * nmemb);
    return size * nmemb;
}
static size_t curl_header_callback(char* buffer, size_t size, size_t nitems, void*) {
    if (size * nitems > 6 && memcmp(buffer, "Date: ", 6) == 0) {
        std::string s (&buffer[6], nitems * size - 6);
        struct tm tm;
        strptime(s.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
        ServerTime::setServerTime(std::chrono::system_clock::from_time_t(timegm(&tm)));
    }
    return nitems * size;
}

std::string RequestBase::sendInternal() const {
    std::string const& url = getUrl();
    std::string body;
    {
        rapidxml::xml_document<char> doc;
        auto dec = doc.allocate_node(rapidxml::node_declaration);
        dec->append_attribute(doc.allocate_attribute("version", "1.0"));
        dec->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
        doc.append_node(dec);

        createBody(doc);
        std::stringstream bodyStream;
        rapidxml::print_to_stream(bodyStream, doc, rapidxml::print_no_indenting);
        body = bodyStream.str();
    }
#ifdef MSA_LOG_NETWORK
    Log::trace("MSANetwork", "Send %s: %s", url.c_str(), body.c_str());
#else
    Log::trace("MSANetwork", "Sending request to: %s", url.c_str());
#endif

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) body.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Expect:");
    headers = curl_slist_append(headers, "Content-type: text/xml; charset=utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header_callback);

    std::stringstream output;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_stringstream_write_func);

    if (CURLManager::initHook != nullptr)
        CURLManager::initHook(curl);
    CURLcode ret = curl_easy_perform(curl);
#ifdef MSA_LOG_NETWORK
    Log::trace("MSANetwork", "Reply (%i): %s", ret, output.str().c_str());
    printf("%s\n", output.str().c_str());
#else
    Log::trace("MSANetwork", "Request complete (%i): %s", ret, url.c_str());
#endif
    return output.str();
}