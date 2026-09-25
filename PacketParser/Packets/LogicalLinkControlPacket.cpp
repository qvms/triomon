#include "LogicalLinkControlPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "IPv4Packet.hpp"
#include "ArpPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

#if __has_include("HpSwitchProtocolPacket.hpp")
#include "HpSwitchProtocolPacket.hpp"
#endif

namespace PacketParser {
namespace Packets {

LogicalLinkControlPacket::LogicalLinkControlPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Logical Link Control (LLC)")
    , organisationCode(0)
    , etherType(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 2 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    dsap = data[packetStartIndex];
    ssap = data[packetStartIndex + 1];
    control = data[packetStartIndex + 2];

    if (dsap == static_cast<uint8_t>(ServiceAccessPointType::SubNetworkAccessProtocol)) {
        if (packetStartIndex + 7 < static_cast<int>(dataLen)) {
            organisationCode = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 3) >> 8;
            etherType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
            if (!parentFrame->QuickParse()) {
                addAttribute("EtherType", QString("0x%1").arg(etherType, 4, 16, QChar('0')));
            }
        }
    } else if (dsap == static_cast<uint8_t>(ServiceAccessPointType::HpExtendedLLC)) {
        if (packetStartIndex + 7 < static_cast<int>(dataLen)) {
            etherType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
            if (!parentFrame->QuickParse()) {
                addAttribute("EtherType", QString("0x%1").arg(etherType, 4, 16, QChar('0')));
            }
        }
    }
}

std::vector<AbstractPacket*> LogicalLinkControlPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    AbstractPacket* packet = nullptr;

    if (dsap == static_cast<uint8_t>(ServiceAccessPointType::SubNetworkAccessProtocol)) {
        if (PacketStartIndex() + 8 < PacketEndIndex()) {
            if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::IPv4)) {
                if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex(), packet)) {
                    packet = nullptr;
                }
            } else if (etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::ARP)) {
                packet = new ArpPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
            } else {
                packet = new RawPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
            }
        }
    } else if (dsap == static_cast<uint8_t>(ServiceAccessPointType::HpExtendedLLC) && etherType == static_cast<uint16_t>(Ethernet2Packet::EtherTypes::HPSW)) {
        if (PacketStartIndex() + 10 < PacketEndIndex()) {
#ifdef __has_include
#if __has_include("HpSwitchProtocolPacket.hpp")
            packet = new HpSwitchProtocolPacket(ParentFrame(), PacketStartIndex() + 3 + 3 + 2 + 2, PacketEndIndex());
#endif
#endif
            if (!packet) packet = new RawPacket(ParentFrame(), PacketStartIndex() + 10, PacketEndIndex());
        }
    } else {
        if (PacketStartIndex() + 3 < PacketEndIndex()) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + 3, PacketEndIndex());
        }
    }

    if (packet) {
        subPackets.push_back(packet);
        std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }

    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
