#include "IEEE_802_1Q_VlanPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "ArpPacket.hpp"
#include "RawPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "../Utils.hpp"

// We forward declare or just include PointToPointOverEthernetPacket and LogicalLinkControlPacket later.
// For now, if they are not defined, we use RawPacket.
#if __has_include("PointToPointOverEthernetPacket.hpp")
#include "PointToPointOverEthernetPacket.hpp"
#endif

#if __has_include("LogicalLinkControlPacket.hpp")
#include "LogicalLinkControlPacket.hpp"
#endif

namespace PacketParser {
namespace Packets {

IEEE_802_1Q_VlanPacket::IEEE_802_1Q_VlanPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "802.1Q VLAN") 
{
    if (packetEndIndex >= packetStartIndex && packetStartIndex + 2 <= parentFrame->DataLength()) {
        priorityTag = static_cast<uint8_t>(parentFrame->Data()[packetStartIndex] >> 5);
        if (!parentFrame->QuickParse()) {
            addAttribute("Priority", QString::number(priorityTag));
        }
        
        vlanID = static_cast<uint16_t>(Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex, false) & 0x0FFF);
        if (!parentFrame->QuickParse()) {
            addAttribute("VLAN ID", QString::number(vlanID));
        }
    }
    if (packetStartIndex + 3 <= static_cast<int>(parentFrame->DataLength())) {
        etherType = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2, false);
    }
}

std::vector<AbstractPacket*> IEEE_802_1Q_VlanPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (PacketStartIndex() + 4 < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        
        if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv4)) {
            if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex(), packet)) {
                packet = nullptr;
            }
        } else if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv6)) {
            packet = new IPv6Packet(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::ARP)) {
            packet = new ArpPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IEEE802_1Q)) {
            packet = new IEEE_802_1Q_VlanPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::PPPoE)) {
#ifdef __has_include
#if __has_include("PointToPointOverEthernetPacket.hpp")
            packet = new PointToPointOverEthernetPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
#endif
#endif
        } else if (etherType < 0x0600) {
#ifdef __has_include
#if __has_include("LogicalLinkControlPacket.hpp")
            packet = new LogicalLinkControlPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
#endif
#endif
        }
        
        if (!packet) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        }
        
        subPackets.push_back(packet);
        std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }
    
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
