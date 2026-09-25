#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class IEEE_802_1Q_VlanPacket : public AbstractPacket {
private:
    uint8_t priorityTag;
    uint16_t vlanID;
    uint16_t etherType;

public:
    IEEE_802_1Q_VlanPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t VlanID() const { return vlanID; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
