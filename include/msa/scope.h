#pragma once

#include <string>

struct MSASecurityScope {
    std::string address;
    std::string policyRef;

    bool operator==(MSASecurityScope const& s) const {
        return address == s.address && policyRef == s.policyRef;
    }
};

namespace std {
    template<>
    struct hash<MSASecurityScope> {
        std::size_t operator()(MSASecurityScope const& o) const {
            std::size_t const addressHash = std::hash<std::string>{}(o.address);
            std::size_t const policyRefHash = std::hash<std::string>{}(o.policyRef);
            return addressHash ^ (policyRefHash << 1);
        }
    };
}