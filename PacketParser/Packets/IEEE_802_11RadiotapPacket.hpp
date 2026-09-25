#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class IEEE_802_11RadiotapPacket : public AbstractPacket {
private:
    uint16_t radiotapHeaderLength;
    uint32_t fieldsPresentFlags;
    uint16_t frequency;
    int signalStrength;

public:
    IEEE_802_11RadiotapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t Frequency() const { return frequency; }
    int SignalStrength() const { return signalStrength; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
