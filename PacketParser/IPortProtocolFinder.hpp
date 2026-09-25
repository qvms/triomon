#pragma once

#include "ApplicationLayerProtocol.hpp"
#include "Packets/IIPPacket.hpp"
#include <cstdint>

namespace PacketParser {

class IPortProtocolFinder {
public:
    virtual ~IPortProtocolFinder() = default;

    virtual ApplicationLayerProtocol getApplicationLayerProtocol(Packets::RFC1700Protocol transport, uint16_t sourcePort, uint16_t destinationPort) = 0;
};

} // namespace PacketParser