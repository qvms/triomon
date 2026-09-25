#include "OpenFlowPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

OpenFlowPacket::OpenFlowPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "OpenFlow")
    , m_version(0), m_type(0), m_length(0), m_parsedBytesCount(0), m_nextPacketIndex(0)
{
    m_version = parentFrame->Data()[packetStartIndex];
    m_type = parentFrame->Data()[packetStartIndex + 1];
    m_length = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
    
    if (m_length > packetEndIndex - packetStartIndex + 1) {
        m_parsedBytesCount = 0;
    } else {
        m_parsedBytesCount = m_length;
        
        if (m_type == static_cast<uint8_t>(OfpType::OFPT_PACKET_IN)) {
            if (m_version == static_cast<uint8_t>(Version::v1_0)) {
                m_nextPacketIndex = packetStartIndex + 18;
            } else if (m_version == static_cast<uint8_t>(Version::v1_1)) {
                m_nextPacketIndex = packetStartIndex + 24;
            } else if (m_version == static_cast<uint8_t>(Version::v1_2)) {
                uint16_t oxmLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 18);
                m_nextPacketIndex = packetStartIndex + 18 + oxmLength + getPadding(oxmLength);
            } else if (m_version == static_cast<uint8_t>(Version::v1_3)) {
                uint16_t oxmLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 26);
                m_nextPacketIndex = packetStartIndex + 26 + oxmLength + getPadding(oxmLength);
            } else {
                if (!ParentFrame()->QuickParse()) const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "OpenFlow version not implemented or unknown"));
            }
        } else if (m_type == static_cast<uint8_t>(OfpType::OFPT_PACKET_OUT)) {
            if (m_version == static_cast<uint8_t>(Version::v1_0)) {
                uint16_t actionLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 14);
                m_nextPacketIndex = packetStartIndex + 16 + actionLength;
            } else {
                uint16_t actionLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 16);
                m_nextPacketIndex = packetStartIndex + 24 + actionLength;
            }
        }
    }
}

int OpenFlowPacket::getPadding(int structLength) const {
    return (8 - (structLength % 8)) % 8;
}

bool OpenFlowPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    if (packetEndIndex - packetStartIndex + 1 < 4) return false;
    
    try {
        uint16_t length = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
        if (length > packetEndIndex - packetStartIndex + 1) return false;
        
        *result = new OpenFlowPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

bool OpenFlowPacket::PacketHeaderIsComplete() const {
    if (m_parsedBytesCount > 0) {
        return PacketEndIndex() - PacketStartIndex() + 1 >= m_parsedBytesCount;
    }
    return false;
}

std::vector<AbstractPacket*> OpenFlowPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (m_nextPacketIndex > 0 && m_nextPacketIndex <= PacketEndIndex()) {
        Ethernet2Packet* packet = new Ethernet2Packet(ParentFrame(), m_nextPacketIndex, PacketEndIndex());
        subPackets.push_back(packet);
        
        std::vector<AbstractPacket*> ethSubPackets = packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), ethSubPackets.begin(), ethSubPackets.end());
    }
    
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
