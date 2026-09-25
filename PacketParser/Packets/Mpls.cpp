#include "Mpls.hpp"
#include "Ethernet2Packet.hpp"
#include "IPv4Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

Mpls::Mpls(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "MPLS")
    , bottomOfStack(false)
    , label(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    bottomOfStack = (data[packetStartIndex + 2] & 0x01) == 0x01;
    label = Utils::ByteConverter::ToUInt32(data, packetStartIndex, true) >> 12;

    if (!parentFrame->QuickParse()) {
        addAttribute("Label", QString::number(label) + " (0x" + QString("%1").arg(label, 4, 16, QChar('0')).toUpper() + ")");
    }
}

std::vector<AbstractPacket*> Mpls::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + PAYLOAD_OFFSET <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;

        if (bottomOfStack) {
            if (PacketStartIndex() + PAYLOAD_OFFSET < static_cast<int>(ParentFrame()->DataLength())) {
                if (ParentFrame()->Data()[PacketStartIndex() + PAYLOAD_OFFSET] < 0x10) {
                    packet = new Ethernet2Packet(ParentFrame(), PacketStartIndex() + PAYLOAD_OFFSET + 4, PacketEndIndex());
                } else {
                    if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + PAYLOAD_OFFSET, PacketEndIndex(), packet)) {
                        packet = nullptr;
                    }
                }
            }
        } else {
            packet = new Mpls(ParentFrame(), PacketStartIndex() + PAYLOAD_OFFSET, PacketEndIndex());
        }

        if (packet) {
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
