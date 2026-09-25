#pragma once

namespace PacketParser {
namespace Packets {

class ISessionPacket {
public:
    virtual ~ISessionPacket() = default;
    virtual bool PacketHeaderIsComplete() const = 0;
    virtual int ParsedBytesCount() const = 0;
};

} // namespace Packets
} // namespace PacketParser
