#pragma once
#include "AbstractPacket.hpp"

namespace PacketParser {
namespace Packets {

class RawPacket : public AbstractPacket {
public:
    RawPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
