#include "RtpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

RtpPacket::RtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "RTP")
{
    if (packetStartIndex + 11 <= packetEndIndex) {
        m_rtpVersion = static_cast<RTPVersion>(parentFrame->Data()[packetStartIndex] >> 6);
        m_padding = (parentFrame->Data()[packetStartIndex] & 0x20) == 0x20;
        m_extension = (parentFrame->Data()[packetStartIndex] & 0x10) == 0x10;
        m_csrcCount = parentFrame->Data()[packetStartIndex] & 0x0f;
        
        m_marker = (parentFrame->Data()[packetStartIndex + 1] >> 7) == 1;
        m_payloadType = parentFrame->Data()[packetStartIndex + 1] & 0x7f;
        
        m_sequenceNumber = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2, false);
        m_sampleTick = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4);
        m_syncSourceID = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 8);
    } else {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "RTP header truncated"));
        }
    }
}

std::vector<AbstractPacket*> RtpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
