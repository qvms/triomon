#pragma once

#include <vector>
#include <cstdint>

namespace PacketHandlerFramework {
namespace Fingerprints {

class AbstractTtlDistanceCalculator {
public:
    virtual ~AbstractTtlDistanceCalculator() = default;

    virtual bool TryGetTtlDistance(uint8_t& ttlDistance, const std::vector<void*>& packetList);
    virtual uint8_t GetTtlDistance(uint8_t ipTimeToLive);
    virtual uint8_t GetOriginalTimeToLive(uint8_t ipTimeToLive);
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
