#include "RawPacket.hpp"

namespace PacketParser {
namespace Packets {

RawPacket::RawPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Unknown")
{
}

std::vector<AbstractPacket*> RawPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
