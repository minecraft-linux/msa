#pragma once

#include <chrono>
#include <atomic>

namespace msa {
namespace network {

class ServerTime {

public:
    static std::atomic<std::chrono::milliseconds> serverTimeOffset;

    static void setServerTime(std::chrono::system_clock::time_point time);

    static std::chrono::system_clock::time_point getServerTime() {
        return std::chrono::system_clock::now() + serverTimeOffset.load();
    }

};

}
}