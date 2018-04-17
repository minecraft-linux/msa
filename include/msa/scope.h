#pragma once

#include <string>

namespace msa {

struct SecurityScope {
    std::string address;
    std::string policyRef;

    bool operator==(SecurityScope const& s) const {
        return address == s.address && policyRef == s.policyRef;
    }
};

}

namespace std {
    template<>
    struct hash<msa::SecurityScope> {
        std::size_t operator()(msa::SecurityScope const& o) const {
            std::size_t const addressHash = std::hash<std::string>{}(o.address);
            std::size_t const policyRefHash = std::hash<std::string>{}(o.policyRef);
            return addressHash ^ (policyRefHash << 1);
        }
    };
}
