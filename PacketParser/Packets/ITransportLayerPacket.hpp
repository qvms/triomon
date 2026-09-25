#pragma once
#include <cstdint>
#include "IPacket.hpp"
#include "IIPPacket.hpp" // For RFC1700Protocol enum

namespace PacketParser {
namespace Packets {

class ITransportLayerPacket : public virtual IPacket {
public:
    virtual ~ITransportLayerPacket() = default;

    virtual uint8_t DataOffsetByteCount() const = 0;
    virtual uint16_t SourcePort() const = 0;
    virtual uint16_t DestinationPort() const = 0;
    virtual uint8_t FlagsRaw() const = 0;
    virtual uint16_t Checksum() const = 0;
    virtual RFC1700Protocol TransportProtocol() const = 0;
};

} // namespace Packets
} // namespace PacketParser
