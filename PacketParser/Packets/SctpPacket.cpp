#include "SctpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

SctpPacket::SctpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "SCTP")
{
    if (packetStartIndex + 11 <= packetEndIndex) {
        m_sourcePort = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex);
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Source Port", QString::number(m_sourcePort));
        }
        
        m_destinationPort = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Destination Port", QString::number(m_destinationPort));
        }
        
        m_verificationTag = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4);
        m_checksum = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 8);
    } else {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "SCTP header truncated"));
        }
    }
}

std::vector<AbstractPacket*> SctpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
