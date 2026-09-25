#include "ErfFrame.hpp"
#include "Ethernet2Packet.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "CiscoHdlcPacket.hpp"
#include "PointToPointPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

ErfFrame::ErfFrame(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "ERF")
    , type(0)
    , extensionHeadersPresent(false)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 8 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    type = data[packetStartIndex + 8] & 0x7f;
    extensionHeadersPresent = (data[packetStartIndex + 8] & 0x80) == 0x80;

    if (!parentFrame->QuickParse()) {
        addAttribute("Type", QString::number(type));
    }
}

std::vector<AbstractPacket*> ErfFrame::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    int erfHeaderLength = 16;
    if (extensionHeadersPresent) {
        erfHeaderLength += 4;
    }

    if (PacketStartIndex() + 16 < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;

        if (type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_ETH) ||
            type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_COLOR_ETH) ||
            type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_DSM_COLOR_ETH)) {
            packet = new Ethernet2Packet(ParentFrame(), PacketStartIndex() + erfHeaderLength + 2, PacketEndIndex());
        } else if (type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_IPV4)) {
            if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + erfHeaderLength, PacketEndIndex(), packet)) {
                packet = nullptr;
            }
        } else if (type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_IPV6)) {
            packet = new IPv6Packet(ParentFrame(), PacketStartIndex() + erfHeaderLength, PacketEndIndex());
        } else if (type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_HDLC_POS) ||
                   type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_COLOR_HDLC_POS) ||
                   type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_DSM_COLOR_HDLC_POS) ||
                   type == static_cast<uint8_t>(RecordTypes::ERF_TYPE_COLOR_MC_HDLC_POS)) {
            uint8_t firstByte = ParentFrame()->Data()[PacketStartIndex()];
            if (firstByte == 0x0f || firstByte == 0x8f) {
                packet = new CiscoHdlcPacket(ParentFrame(), PacketStartIndex() + erfHeaderLength, PacketEndIndex());
            } else {
                packet = new PointToPointPacket(ParentFrame(), PacketStartIndex() + erfHeaderLength, PacketEndIndex());
            }
        }

        if (!packet) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + erfHeaderLength, PacketEndIndex());
        }
        
        subPackets.push_back(packet);
        std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
