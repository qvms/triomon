#pragma once

namespace PacketParser {

class Frame;

namespace Packets {

class IPacket {
public:
    virtual ~IPacket() = default;

    virtual const Frame* ParentFrame() const = 0;
    virtual int PacketStartIndex() const = 0;
};

} // namespace Packets
} // namespace PacketParser
