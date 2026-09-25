#include "GtpPacket.hpp"
#include "../../SharedUtils/Utils.hpp"

namespace PacketParser {
namespace Packets {

GtpPacket::GtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "GTP"),
      m_flagData(0), m_version(0), m_messageType(0), m_messageLength(0), m_tunnelEndpointID(0)
{
    if (packetEndIndex >= packetStartIndex + 7) {
        m_version = parentFrame->Data()[packetStartIndex] >> 5;
        m_flagData = parentFrame->Data()[packetStartIndex];
        m_messageType = parentFrame->Data()[packetStartIndex + 1];
        m_messageLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2, false);
        
        int offset = 4;
        bool hasTeidV2 = (m_version == 2) && ((m_flagData & (1 << 3)) != 0);
        if (m_version == 1 || hasTeidV2) {
            if (packetStartIndex + offset + 3 <= packetEndIndex) {
                m_tunnelEndpointID = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4);
                offset += 4;
            }
        }
        
        bool hasExtV1 = (m_version == 1) && ((m_flagData & (1 << 2)) != 0);
        bool hasSeqV1 = (m_version == 1) && ((m_flagData & (1 << 1)) != 0);
        bool hasNpduV1 = (m_version == 1) && ((m_flagData & 1) != 0);
        
        if (hasExtV1) {
            offset += 3; // skip seq and n-pdu
            if (packetStartIndex + offset <= packetEndIndex) {
                uint8_t nextExtensionHeaderType = parentFrame->Data()[packetStartIndex + offset];
                offset++;
                while (nextExtensionHeaderType != 0 && packetStartIndex + offset <= packetEndIndex) {
                    int extensionLengthInBytes = 4 * parentFrame->Data()[packetStartIndex + offset];
                    if (extensionLengthInBytes < 4) break; // avoid infinite loop
                    offset += extensionLengthInBytes - 1;
                    if (packetStartIndex + offset > packetEndIndex) break;
                    nextExtensionHeaderType = parentFrame->Data()[packetStartIndex + offset];
                    offset++;
                }
            }
        } else if (hasSeqV1 || hasNpduV1) {
            offset += 4;
        }
        
        if (m_version == 1 && m_messageType == 255) { // GPDU
            int tunneledPacketLength = m_messageLength - offset + 8;
            if (tunneledPacketLength > 0 && packetStartIndex + offset + tunneledPacketLength - 1 <= packetEndIndex) {
                m_tunneledPacketInfo = std::make_pair(offset, tunneledPacketLength);
            }
        }
    }
}

std::vector<AbstractPacket*> GtpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference)
        subPackets.push_back(this);
        
    if (m_tunneledPacketInfo.has_value()) {
        int payloadStartIndex = PacketStartIndex() + m_tunneledPacketInfo.value().first;
        int payloadEndIndex = payloadStartIndex + m_tunneledPacketInfo.value().second - 1;
        
        AbstractPacket* payloadPacket = nullptr;
        if (!IPv4Packet::TryParse(ParentFrame(), payloadStartIndex, payloadEndIndex, payloadPacket)) {
            try {
                payloadPacket = new IPv6Packet(ParentFrame(), payloadStartIndex, payloadEndIndex);
            } catch (...) {
                payloadPacket = nullptr;
            }
        }
        
        if (payloadPacket) {
            subPackets.push_back(payloadPacket);
            std::vector<AbstractPacket*> childSubPackets = payloadPacket->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
