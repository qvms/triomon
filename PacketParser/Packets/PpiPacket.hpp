#pragma once
#include "AbstractPacket.hpp"
#include "../PcapFrameDataLinkTypeEnum.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class PpiPacket : public AbstractPacket {
private:
    uint16_t ppiLength;
    DataLinkTypeEnum dataLinkType;

public:
    PpiPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
