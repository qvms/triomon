#pragma once
#include <QString>
#include <cstdint>
#include "IPacket.hpp"

namespace PacketParser {
namespace Packets {

enum class RFC1700Protocol : uint8_t {
    HOPOPT = 0x00,
    ICMP = 0x01,
    IGMP = 0x02,
    IPv4 = 0x04,
    TCP = 0x06,
    UDP = 0x11,
    IPv6 = 0x29,
    RSVP = 0x2e,
    GRE = 0x2f,
    ESP = 0x32,
    ICMPv6 = 0x3a,
    OSPF = 0x59,
    SCTP = 0x84,
    Unknown = 0xff
};

class IIPPacket : public virtual IPacket {
public:
    virtual ~IIPPacket() = default;

    virtual QString SourceIPAddress() const = 0;
    virtual QString DestinationIPAddress() const = 0;
    virtual int PayloadLength() const = 0;
    virtual uint8_t HeaderLength() const = 0;
    virtual uint8_t HopLimit() const = 0;
    virtual uint8_t NextRFC1700Protocol() const = 0;
};

} // namespace Packets
} // namespace PacketParser
