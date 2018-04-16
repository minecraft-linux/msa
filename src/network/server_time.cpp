#include <msa/network/server_time.h>

using namespace msa::network;

std::chrono::milliseconds ServerTime::serverTimeOffset;

void ServerTime::setServerTime(std::chrono::system_clock::time_point time) {
    auto localTime = std::chrono::system_clock::now();
    serverTimeOffset = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) -
            std::chrono::duration_cast<std::chrono::milliseconds>(localTime.time_since_epoch());
}