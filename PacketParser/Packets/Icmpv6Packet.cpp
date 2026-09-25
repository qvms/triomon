#include "Icmpv6Packet.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

uint8_t Icmpv6Packet::GetReverseType(uint8_t type, uint8_t code, bool& unidirectional) {
    unidirectional = false;
    switch (type) {
        case 128: return 129; // echo => echo reply
        case 129: return 128;
        case 130: return 131; // MLD_LISTENER => Report
        case 131: return 130;
        case 133: return 134; // ROUTER_SOLICIT => ADVERT
        case 134: return 133;
        case 135: return 136; // NEIGHBOR_SOLICIT => ADVERT
        case 136: return 135;
        case 139: return 140; // WRU_REQUEST => REPLY
        case 140: return 139;
        case 144: return 145; // HAAD_REQUEST => REPLY
        case 145: return 144;
        default:
            unidirectional = true;
            return code;
    }
}

bool Icmpv6Packet::IsUnidirectionalType(uint8_t type) {
    bool unidirectional = false;
    GetReverseType(type, 0, unidirectional);
    return unidirectional;
}

Icmpv6Packet::Icmpv6Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "ICMPv6")
{
    const uint8_t* data = parentFrame->Data();
    if (packetStartIndex + 2 >= static_cast<int>(parentFrame->DataLength())) return;

    m_type = data[packetStartIndex];
    m_code = data[packetStartIndex + 1];
    
    if (packetStartIndex + 4 <= static_cast<int>(parentFrame->DataLength())) {
        m_checksum = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    } else {
        m_checksum = 0;
    }
}

std::vector<AbstractPacket*> Icmpv6Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
