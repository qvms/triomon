#include "AbstractTtlDistanceCalculator.hpp"

namespace PacketHandlerFramework {
namespace Fingerprints {

bool AbstractTtlDistanceCalculator::TryGetTtlDistance(uint8_t& ttlDistance, const std::vector<void*>& packetList) {
    // Stub: Without full Packet framework, we can't inspect packet types easily here
    // In full port, we'd check if packet is IPv4Packet and extract TTL.
    ttlDistance = 0;
    return false;
}

uint8_t AbstractTtlDistanceCalculator::GetTtlDistance(uint8_t ipTimeToLive) {
    return GetOriginalTimeToLive(ipTimeToLive) - ipTimeToLive;
}

uint8_t AbstractTtlDistanceCalculator::GetOriginalTimeToLive(uint8_t ipTimeToLive) {
    if (ipTimeToLive > 128) {
        return 255;
    } else if (ipTimeToLive > 64) {
        return 128;
    } else if (ipTimeToLive > 32) {
        return 64;
    } else {
        return 32;
    }
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
