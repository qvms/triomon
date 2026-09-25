#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class PointToPointOverEthernetPacket : public AbstractPacket {
private:
    uint8_t code;
    uint16_t payloadLength;

public:
    PointToPointOverEthernetPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
