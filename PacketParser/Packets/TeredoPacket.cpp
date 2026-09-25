#include "TeredoPacket.hpp"
#include "../../SharedUtils/Utils.hpp"

namespace PacketParser {
namespace Packets {

TeredoPacket::TeredoPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Teredo"),
      m_ipv6Packet(nullptr)
{
    int offset = 0;
    if (packetStartIndex + offset < packetEndIndex) {
        if (Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + offset) == 0x0000) {
            // Origin indication
            offset += 8;
        } else if (parentFrame->Data()[packetStartIndex + offset] == 0x01) {
            // Authentication
            uint8_t authLength = parentFrame->Data()[packetStartIndex + offset + 1];
            offset += authLength + 2;
        }
    }
    
    if (packetStartIndex + offset + 40 <= packetEndIndex) {
        if ((parentFrame->Data()[packetStartIndex + offset] >> 4) == 0x06) {
            try {
                m_ipv6Packet = new IPv6Packet(parentFrame, packetStartIndex + offset, packetEndIndex);
            } catch (...) {
                m_ipv6Packet = nullptr;
            }
        }
    }
}

std::vector<AbstractPacket*> TeredoPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference)
        subPackets.push_back(this);
        
    if (m_ipv6Packet) {
        subPackets.push_back(m_ipv6Packet);
        std::vector<AbstractPacket*> childSubPackets = m_ipv6Packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }
    
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
