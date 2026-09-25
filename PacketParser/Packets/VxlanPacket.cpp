#include "VxlanPacket.hpp"
#include "../../SharedUtils/Utils.hpp"

namespace PacketParser {
namespace Packets {

VxlanPacket::VxlanPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "VXLAN"),
      m_innerEthernetPacket(nullptr)
{
    if (packetEndIndex >= packetStartIndex + 15) { // 16 bytes min 
        m_vxlanNetworkIdentifier = static_cast<int>(Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4, 3));
    }
}

std::vector<AbstractPacket*> VxlanPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference)
        subPackets.push_back(this);
        
    if (m_innerEthernetPacket) {
        subPackets.push_back(m_innerEthernetPacket);
    } else if (m_vxlanNetworkIdentifier.has_value()) {
        if (PacketStartIndex() + 8 <= PacketEndIndex()) {
            m_innerEthernetPacket = new Ethernet2Packet(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
            subPackets.push_back(m_innerEthernetPacket);
            std::vector<AbstractPacket*> childSubPackets = m_innerEthernetPacket->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
