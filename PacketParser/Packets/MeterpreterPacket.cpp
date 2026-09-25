#include "MeterpreterPacket.hpp"

namespace PacketParser {
namespace Packets {

MeterpreterPacket::MeterpreterPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Meterpreter")
    , PayloadLength(0)
    , HasMZHeader(false)
{
    if (PacketLength() == 4 && parentFrame->Data()[packetStartIndex + 3] == 0x00) {
        PayloadLength = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex, 4, true);
    } else if (PacketLength() > 1 && Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex) == 0x4d5a) {
        HasMZHeader = true;
    } else {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Packet is not Meterpreter"));
        }
    }
}

bool MeterpreterPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    try {
        if (packetEndIndex - packetStartIndex + 1 == 4 || 
            (packetEndIndex - packetStartIndex > 2 && Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex) == 0x4d5a)) {
            *result = new MeterpreterPacket(parentFrame, packetStartIndex, packetEndIndex);
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

std::vector<AbstractPacket*> MeterpreterPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
