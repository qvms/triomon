#include "UmasPacket.hpp"
#include <QDebug>

namespace PacketParser {
namespace Packets {

UmasPacket::UmasPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, ModbusTcpPacket* modbusTcpPacket)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "UMAS")
    , ParentModbusPacket(modbusTcpPacket)
    , SessionKey(0)
    , UmasFunctionCode(static_cast<FC>(0))
    , HasUmasFunctionCode(false)
    , UmasFunctionCodeRaw(0)
{
    if (packetStartIndex + 1 <= packetEndIndex) {
        SessionKey = parentFrame->Data()[packetStartIndex];
        UmasFunctionCodeRaw = parentFrame->Data()[packetStartIndex + 1];
        
        switch (UmasFunctionCodeRaw) {
            case 0x01: case 0x02: case 0x03: case 0x04: case 0x06: case 0x07: case 0x0A: case 0x10:
            case 0x11: case 0x12: case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
            case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x30: case 0x31: case 0x32:
            case 0x33: case 0x34: case 0x35: case 0x36: case 0x37: case 0x39: case 0x40: case 0x41:
            case 0x50: case 0x58: case 0x60: case 0x6d: case 0x6e: case 0x70: case 0x71: case 0x73:
            case 0xFE: case 0xFD:
                UmasFunctionCode = static_cast<FC>(UmasFunctionCodeRaw);
                HasUmasFunctionCode = true;
                break;
            default:
                break;
        }
    }
}

std::vector<AbstractPacket*> UmasPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
