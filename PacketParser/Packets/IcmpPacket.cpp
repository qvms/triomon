#include "IcmpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

uint8_t IcmpPacket::GetReverseType(uint8_t type, uint8_t code, bool& unidirectional) {
    unidirectional = false;
    switch (type) {
        case 0: return 8;  // echo reply => echo
        case 8: return 0;  // echo => echo reply
        case 9: return 10; // Router advertisment => Router solicit
        case 10: return 9; // Router solicit => Router advertisment
        case 13: return 14;// Timestamp => Timestamp reply
        case 14: return 13;// Timestamp reply => Timestamp
        case 15: return 16;// Info => Info reply
        case 16: return 15;// Info reply => Info
        case 17: return 18;// Mask => Mask reply
        case 18: return 17;// Mask reply => Mask
        default:
            unidirectional = true;
            return code;
    }
}

bool IcmpPacket::IsUnidirectionalType(uint8_t type) {
    bool unidirectional = false;
    GetReverseType(type, 0, unidirectional);
    return unidirectional;
}

IcmpPacket::IcmpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "ICMP")
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

std::vector<AbstractPacket*> IcmpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
