#pragma once
#include <chrono>
#include <tuple>
#include <cstdint>
#include <optional>

namespace SharedUtils {
    namespace Pcap {
        enum class TimestampResolution {
            microsecond,
            nanosecond
        };
    }

class DateTimeUtils {
public:
    static std::chrono::system_clock::time_point EpochToDateTime(uint32_t seconds, std::optional<std::pair<Pcap::TimestampResolution, uint32_t>> subSeconds = std::nullopt) {
        long long ticks = static_cast<long long>(seconds) * 10000000;
        if (subSeconds.has_value()) {
            if (subSeconds->first == Pcap::TimestampResolution::microsecond) {
                ticks += subSeconds->second * 10;
            } else if (subSeconds->first == Pcap::TimestampResolution::nanosecond) {
                ticks += subSeconds->second / 100;
            }
        }
        return std::chrono::system_clock::time_point(std::chrono::system_clock::duration(ticks)); // Since C++ epoch is usually 1970
    }

    static std::pair<uint32_t, uint32_t> DateTimeToEpoch(std::chrono::system_clock::time_point timestamp) {
        auto duration = timestamp.time_since_epoch();
        long long totalMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        uint32_t seconds = static_cast<uint32_t>(totalMicroseconds / 1000000);
        uint32_t microseconds = static_cast<uint32_t>(totalMicroseconds % 1000000);
        return {seconds, microseconds};
    }
};

}