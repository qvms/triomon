#include "ArpPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

ArpPacket::ArpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "ARP")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLength = parentFrame->DataLength();

    if (packetStartIndex + 8 > static_cast<int>(dataLength)) return;

    m_hardwareType = Utils::ByteConverter::ToUInt16(data, packetStartIndex);
    if (m_hardwareType != 1 && !ParentFrame()->QuickParse()) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex + 1, "ARP HardwareType not Ethernet"));
    }

    m_protocolType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    if (m_protocolType != static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv4) && !ParentFrame()->QuickParse()) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 2, packetStartIndex + 3, "ARP ProtocolType not IPv4"));
    }

    m_hardwareLength = data[packetStartIndex + 4];
    if (m_hardwareLength != 6 && !ParentFrame()->QuickParse()) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 4, packetStartIndex + 4, "ARP HardwareLength<>6 (not Ethernet)"));
    }

    m_protocolLength = data[packetStartIndex + 5];
    if (m_protocolLength != 4 && !ParentFrame()->QuickParse()) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 5, packetStartIndex + 5, QString("ARP ProtocolLength<>4 (not IPv4) (it is: %1)").arg(m_protocolLength)));
    }

    m_operation = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
    if (m_operation != 1 && m_operation != 2 && !ParentFrame()->QuickParse()) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 6, packetStartIndex + 7, "ARP Operation not Request nor Reply"));
    }

    int expectedLength = 8 + 2 * m_hardwareLength + 2 * m_protocolLength;
    if (packetStartIndex + expectedLength > static_cast<int>(dataLength)) {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex + expectedLength, "Packet too short for ARP payload"));
        }
        return;
    }

    m_senderHardwareAddress.assign(data + packetStartIndex + 8, data + packetStartIndex + 8 + m_hardwareLength);
    m_senderProtocolAddress.assign(data + packetStartIndex + 8 + m_hardwareLength, data + packetStartIndex + 8 + m_hardwareLength + m_protocolLength);
    m_targetHardwareAddress.assign(data + packetStartIndex + 8 + m_hardwareLength + m_protocolLength, data + packetStartIndex + 8 + 2 * m_hardwareLength + m_protocolLength);
    m_targetProtocolAddress.assign(data + packetStartIndex + 8 + 2 * m_hardwareLength + m_protocolLength, data + packetStartIndex + 8 + 2 * m_hardwareLength + 2 * m_protocolLength);
}

QHostAddress ArpPacket::SenderIPAddress() const {
    if (m_senderProtocolAddress.size() == 4) {
        return QHostAddress(m_senderProtocolAddress[0] << 24 | m_senderProtocolAddress[1] << 16 | m_senderProtocolAddress[2] << 8 | m_senderProtocolAddress[3]);
    }
    return QHostAddress();
}

QHostAddress ArpPacket::TargetIPAddress() const {
    if (m_targetProtocolAddress.size() == 4) {
        return QHostAddress(m_targetProtocolAddress[0] << 24 | m_targetProtocolAddress[1] << 16 | m_targetProtocolAddress[2] << 8 | m_targetProtocolAddress[3]);
    }
    return QHostAddress();
}

std::vector<AbstractPacket*> ArpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
