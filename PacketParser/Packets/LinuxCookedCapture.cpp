#include "LinuxCookedCapture.hpp"
#include "Ethernet2Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"
#include <QStringList>

namespace PacketParser {
namespace Packets {

LinuxCookedCapture::LinuxCookedCapture(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Linux cooked capture (SLL)")
    , packetType(0)
    , addressType(0)
    , addressLength(0)
    , protocol(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 15 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    packetType = Utils::ByteConverter::ToUInt16(data, packetStartIndex, true);

    if (!parentFrame->QuickParse()) {
        if (packetType == static_cast<uint16_t>(PacketTypes::LINUX_SLL_HOST)) {
            addAttribute("Packet Type", "Unicast to us (HOST)");
        } else if (packetType == static_cast<uint16_t>(PacketTypes::LINUX_SLL_BROADCAST)) {
            addAttribute("Packet Type", "Broadcast");
        } else if (packetType == static_cast<uint16_t>(PacketTypes::LINUX_SLL_MULTICAST)) {
            addAttribute("Packet Type", "Multicast");
        } else if (packetType == static_cast<uint16_t>(PacketTypes::LINUX_SLL_OTHERHOST)) {
            addAttribute("Packet Type", "Unicast to another host (OTHERHOST)");
        } else if (packetType == static_cast<uint16_t>(PacketTypes::LINUX_SLL_OUTGOING)) {
            addAttribute("Packet Type", "Sent by us (OUTGOING)");
        }
    }

    addressType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, true);
    addressLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4, true);

    if (addressLength == 6) {
        if (packetStartIndex + 11 < static_cast<int>(dataLen)) {
            QStringList parts;
            for (int i = 0; i < 6; ++i) {
                parts << QString("%1").arg(data[packetStartIndex + 6 + i], 2, 16, QChar('0')).toUpper();
            }
            sourceMacAddress = parts.join("-");
        }
    } else {
        sourceMacAddress = "None";
    }

    protocol = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 14, true);
}

std::vector<AbstractPacket*> LinuxCookedCapture::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + SLL_HEADER_LENGTH < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (Ethernet2Packet::TryGetPacketForType(protocol, ParentFrame(), PacketStartIndex() + SLL_HEADER_LENGTH, PacketEndIndex(), packet)) {
            if (packet) {
                subPackets.push_back(packet);
                std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
                subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
            }
        } else {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + SLL_HEADER_LENGTH, PacketEndIndex());
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
