#include "Ethernet2Packet.hpp"
#include "RawPacket.hpp"
#include "IEEE_802_1Q_VlanPacket.hpp"
#include "Mpls.hpp"
#include "Erspan.hpp"
#include "PointToPointOverEthernetPacket.hpp"
#include "LogicalLinkControlPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "ArpPacket.hpp"

#include <QDebug>
#include <QStringList>

namespace PacketParser {
namespace Packets {

bool Ethernet2Packet::TryGetPacketForType(uint16_t etherType, const Frame* parentFrame, int newPacketStartIndex, int newPacketEndIndex, AbstractPacket*& packet) {
    // Enum.IsDefined check in C# is simplified here
    packet = GetPacketForType(etherType, parentFrame, newPacketStartIndex, newPacketEndIndex);
    return packet != nullptr;
}

AbstractPacket* Ethernet2Packet::GetPacketForType(uint16_t etherType, const Frame* parentFrame, int newPacketStartIndex, int newPacketEndIndex) {
    AbstractPacket* packet = nullptr;
    try {
        if (etherType == static_cast<uint16_t>(EtherTypes::IPv4)) {
            if (!IPv4Packet::TryParse(parentFrame, newPacketStartIndex, newPacketEndIndex, packet)) {
                packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
            }
        } else if (etherType == static_cast<uint16_t>(EtherTypes::IPv6)) {
            packet = new IPv6Packet(parentFrame, newPacketStartIndex, newPacketEndIndex);
        } else if (etherType == static_cast<uint16_t>(EtherTypes::ARP)) {
            packet = new ArpPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
                } else if (etherType == static_cast<uint16_t>(EtherTypes::IEEE802_1Q)) {
#if __has_include("IEEE_802_1Q_VlanPacket.hpp")
            packet = new IEEE_802_1Q_VlanPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#else
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#endif
        } else if (etherType == static_cast<uint16_t>(EtherTypes::MPLS)) {
#if __has_include("Mpls.hpp")
            packet = new Mpls(parentFrame, newPacketStartIndex, newPacketEndIndex);
#else
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#endif
        } else if (etherType == static_cast<uint16_t>(EtherTypes::ERSPAN)) {
#if __has_include("Erspan.hpp")
            packet = new Erspan(parentFrame, newPacketStartIndex, newPacketEndIndex);
#else
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#endif
        } else if (etherType == static_cast<uint16_t>(EtherTypes::PPPoE)) {
#if __has_include("PointToPointOverEthernetPacket.hpp")
            packet = new PointToPointOverEthernetPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#else
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#endif
        } else if (etherType == static_cast<uint16_t>(EtherTypes::xHayesTunnel)) {
            packet = new Ethernet2Packet(parentFrame, newPacketStartIndex + 4, newPacketEndIndex);
        } else if (etherType == static_cast<uint16_t>(EtherTypes::TransparentEthernet)) {
            packet = new Ethernet2Packet(parentFrame, newPacketStartIndex, newPacketEndIndex);
        } else if (etherType < static_cast<uint16_t>(EtherTypes::IEEE802_3_Max)) {
#if __has_include("LogicalLinkControlPacket.hpp")
            packet = new LogicalLinkControlPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#else
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
#endif
        } else {
            // Unimplemented subclasses fall back to RawPacket for now
            packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
        }
    } catch (...) {
        packet = new RawPacket(parentFrame, newPacketStartIndex, newPacketEndIndex);
    }
    return packet;
}

Ethernet2Packet::Ethernet2Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Ethernet2")
{
    if (packetStartIndex + 14 > static_cast<int>(parentFrame->DataLength())) {
        return; // Quick bounds check
    }

    const uint8_t* data = parentFrame->Data();
    m_etherType = (data[packetStartIndex + 12] << 8) | data[packetStartIndex + 13];

    m_destinationMAC = ConvertToHexString(data + packetStartIndex, 6);
    if (!parentFrame->QuickParse()) {
        addAttribute("Destination MAC", m_destinationMAC);
    }

    m_sourceMAC = ConvertToHexString(data + packetStartIndex + 6, 6);
    if (!parentFrame->QuickParse()) {
        addAttribute("Source MAC", m_sourceMAC);
    }
}

QString Ethernet2Packet::ConvertToHexString(const uint8_t* data, size_t len) {
    QStringList parts;
    for (size_t i = 0; i < len; ++i) {
        parts << QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return parts.join("-");
}

std::vector<AbstractPacket*> Ethernet2Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    
    if (PacketStartIndex() + 14 < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (TryGetPacketForType(m_etherType, ParentFrame(), PacketStartIndex() + 14, PacketEndIndex(), packet) && packet != nullptr) {
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
