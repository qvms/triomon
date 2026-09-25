#include "Erspan.hpp"
#include "Ethernet2Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

Erspan::Erspan(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "MPLS") // Label was MPLS in C# code probably by copy-paste mistake, but I must follow 1:1!
    , erspanHeaderLength(8)
    , m_version(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 1 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    m_version = data[packetStartIndex] >> 4;
    
    if ((data[packetStartIndex] & 0x18) == 0x18) {
        m_vlanID = std::nullopt;
    } else {
        m_vlanID = Utils::ByteConverter::ToUInt16(data, packetStartIndex, false) & 0xFFF;
    }

    if (m_version == 1) {
        erspanHeaderLength = 8;
    } else if (m_version == 2) {
        erspanHeaderLength = 12;
        if (packetStartIndex + 11 < static_cast<int>(dataLen)) {
            if ((data[packetStartIndex + 11] & 1) != 0) {
                erspanHeaderLength += 8;
            }
        }
    }
}

std::vector<AbstractPacket*> Erspan::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + erspanHeaderLength <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        try {
            packet = new Ethernet2Packet(ParentFrame(), PacketStartIndex() + erspanHeaderLength, PacketEndIndex());
        } catch (...) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + erspanHeaderLength, PacketEndIndex());
        }

        if (packet) {
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }

    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
