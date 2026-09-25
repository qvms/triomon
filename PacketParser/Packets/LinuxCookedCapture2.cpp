#include "LinuxCookedCapture2.hpp"
#include "Ethernet2Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

LinuxCookedCapture2::LinuxCookedCapture2(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Linux cooked capture v2 (SLL2)")
    , protocol(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 1 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    protocol = Utils::ByteConverter::ToUInt16(data, packetStartIndex, true);

    if (!parentFrame->QuickParse()) {
        addAttribute("Ether Type", QString::number(protocol));
    }
}

std::vector<AbstractPacket*> LinuxCookedCapture2::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + SLL2_HEADER_LENGTH <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (Ethernet2Packet::TryGetPacketForType(protocol, ParentFrame(), PacketStartIndex() + SLL2_HEADER_LENGTH, PacketEndIndex(), packet)) {
            if (packet) {
                subPackets.push_back(packet);
                std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
                subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
            }
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
