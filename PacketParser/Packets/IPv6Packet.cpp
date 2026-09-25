#include "IPv6Packet.hpp"
#include "IPv4Packet.hpp" // For TryGetSubPacket
#include "RawPacket.hpp"
#include "../Utils.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

HopByHopOption::HopByHopOption(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IPv6 Hop-by-Hop Option")
    , NextHeaderRFC1700Protocol(parentFrame->Data()[packetStartIndex])
{
    uint8_t length = parentFrame->Data()[packetStartIndex + 1];
    m_nextHeaderStartOffset = 8 + (8 * length);
}

std::vector<AbstractPacket*> HopByHopOption::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + m_nextHeaderStartOffset < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (IPv4Packet::TryGetSubPacket(NextHeaderRFC1700Protocol, ParentFrame(), PacketStartIndex() + m_nextHeaderStartOffset, PacketEndIndex(), packet)) {
            subPackets.push_back(packet);
        } else {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + m_nextHeaderStartOffset, PacketEndIndex());
            subPackets.push_back(packet);
        }

        if (packet) {
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}


IPv6Packet::IPv6Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IPv6")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLength = parentFrame->DataLength();

    if (packetStartIndex + 40 > static_cast<int>(dataLength)) return;

    if (!ParentFrame()->QuickParse()) {
        if ((data[packetStartIndex] >> 4) != 0x06) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, QString("IP Version!=6 (%1)").arg(data[packetStartIndex] >> 4)));
        }
    }

    m_payloadLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);
    
    if (PacketEndIndex() > PacketStartIndex() + HeaderLength() + m_payloadLength - 1) {
        setPacketEndIndex(PacketStartIndex() + HeaderLength() + m_payloadLength - 1);
    }

    m_nextHeader = data[packetStartIndex + 6];
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Next Header", QString("0x%1").arg(m_nextHeader, 2, 16, QChar('0')).toUpper());
    }

    m_hopLimit = data[packetStartIndex + 7];
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Hop Limit", QString::number(m_hopLimit));
    }

    // Source IP
    m_sourceIP = QHostAddress(data + packetStartIndex + 8);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Source IP", m_sourceIP.toString());
    }

    // Destination IP
    m_destinationIP = QHostAddress(data + packetStartIndex + 24);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Destination IP", m_destinationIP.toString());
    }
}

std::vector<AbstractPacket*> IPv6Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + IPV6_HEADER_LENGTH < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (IPv4Packet::TryGetSubPacket(m_nextHeader, ParentFrame(), PacketStartIndex() + IPV6_HEADER_LENGTH, PacketEndIndex(), packet)) {
            subPackets.push_back(packet);
        } else {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + IPV6_HEADER_LENGTH, PacketEndIndex());
            subPackets.push_back(packet);
        }

        if (packet) {
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
