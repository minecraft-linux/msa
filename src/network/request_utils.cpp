#include <msa/network/request_utils.h>
#include <sstream>

using namespace msa::network;

std::string RequestUtils::urlEscape(std::string const& str) {
    // first check how big the result string should be
    size_t reslen = 0;
    for (char c : str) {
        reslen++;
        if (!isUrlSafe(c))
            reslen += 2;
    }

    std::string ret;
    ret.resize(reslen);
    size_t off = 0;
    for (char c : str) {
        if (isUrlSafe(c)) {
            ret[off++] = c;
        } else {
            ret[off++] = '%';
            sprintf(&ret[off], "%02X", c);
            off += 2;
        }
    }
    return ret;
}

std::string RequestUtils::encodeUrlParams(std::vector<std::pair<std::string, std::string>> const& data) {
    std::stringstream ss;
    bool first = true;
    for (auto const& p : data) {
        if (!first)
            ss << "&";
        ss << urlEscape(p.first) << "=" << urlEscape(p.second);
        first = false;
    }
    return ss.str();
}