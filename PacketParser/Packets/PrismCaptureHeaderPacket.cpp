#include "PrismCaptureHeaderPacket.hpp"
#include "IEEE_802_11Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

PrismCaptureHeaderPacket::PrismCaptureHeaderPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Prism capture header")
    , messageLength(0)
    , channel(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 59 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    messageLength = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4, true);
    
    device = Utils::ByteConverter::ReadString(data, packetStartIndex + 8, 16, false, false);
    if (!parentFrame->QuickParse()) {
        addAttribute("Device", device);
    }

    channel = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 56, true);
    if (!parentFrame->QuickParse()) {
        addAttribute("Channel", QString::number(channel));
    }
}

std::vector<AbstractPacket*> PrismCaptureHeaderPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + 144 < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        try {
            packet = new IEEE_802_11Packet(ParentFrame(), PacketStartIndex() + 144, PacketEndIndex());
        } catch (...) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + 144, PacketEndIndex());
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
