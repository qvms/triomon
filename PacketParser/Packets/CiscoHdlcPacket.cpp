#include "CiscoHdlcPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "ArpPacket.hpp"
#include "IEEE_802_1Q_VlanPacket.hpp"
#include "PointToPointOverEthernetPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

CiscoHdlcPacket::CiscoHdlcPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Cisco HDLC")
    , protocolCode(0)
    , hasProtocolCode(false)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();
    
    if (packetStartIndex + 4 <= static_cast<int>(dataLen) && packetStartIndex + 3 <= packetEndIndex) {
        protocolCode = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
        hasProtocolCode = true;
    }
}

std::vector<AbstractPacket*> CiscoHdlcPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (PacketStartIndex() + 4 < PacketEndIndex() && hasProtocolCode) {
        AbstractPacket* packet = nullptr;
        
        if (protocolCode == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv4)) {
            if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex(), packet)) {
                packet = nullptr;
            }
        } else if (protocolCode == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv6)) {
            packet = new IPv6Packet(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (protocolCode == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::ARP)) {
            packet = new ArpPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (protocolCode == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IEEE802_1Q)) {
            packet = new IEEE_802_1Q_VlanPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        } else if (protocolCode == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::PPPoE)) {
            packet = new PointToPointOverEthernetPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
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
