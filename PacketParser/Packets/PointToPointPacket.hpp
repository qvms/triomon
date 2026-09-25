#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class PointToPointPacket : public AbstractPacket {
private:
    uint16_t protocol;
    int protocolStartOffset;

public:
    PointToPointPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
