#pragma once
#include "AbstractPacket.hpp"
#include "../PcapFrameDataLinkTypeEnum.hpp"

namespace PacketParser {
namespace Packets {

class PacketFactory {
public:
    static bool TryGetPacket(AbstractPacket*& packet, DataLinkTypeEnum dataLinkType, const Frame* parentFrame, int startIndex, int endIndex);
};

} // namespace Packets
} // namespace PacketParser
