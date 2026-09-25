#include "IEEE_802_11RadiotapPacket.hpp"
#include "IEEE_802_11Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"
#include <cmath>

namespace PacketParser {
namespace Packets {

IEEE_802_11RadiotapPacket::IEEE_802_11RadiotapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IEEE 802.11 Radiotap")
    , radiotapHeaderLength(0)
    , fieldsPresentFlags(0)
    , frequency(0)
    , signalStrength(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    radiotapHeaderLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, true);
    if (!parentFrame->QuickParse()) {
        addAttribute("Header length", QString::number(radiotapHeaderLength));
    }

    if (packetStartIndex + 7 >= static_cast<int>(dataLen)) return;

    fieldsPresentFlags = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4, true);

    int offset = packetStartIndex + 8;
    for (int i = 0; i < 8; i++) {
        if ((fieldsPresentFlags & (1 << i)) != 0) {
            if (i == 0) offset += 8;
            else if (i == 1) offset += 1;
            else if (i == 2) offset += 1;
            else if (i == 3) {
                if (offset + 1 < static_cast<int>(dataLen)) {
                    frequency = Utils::ByteConverter::ToUInt16(data, offset, true);
                    if (!parentFrame->QuickParse()) {
                        addAttribute("Frequency", QString::number(frequency) + " MHz");
                    }
                }
                offset += 4;
            } else if (i == 4) offset += 2;
            else if (i == 5) {
                if (offset < static_cast<int>(dataLen)) {
                    signalStrength = static_cast<int8_t>(data[offset]);
                    if (!parentFrame->QuickParse()) {
                        double mw = std::pow(10.0, signalStrength / 10.0);
                        addAttribute("Signal strength", QString("%1 dBm (%2 mW)").arg(signalStrength).arg(mw));
                    }
                }
                offset += 1;
            }
        }
    }
}

std::vector<AbstractPacket*> IEEE_802_11RadiotapPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + radiotapHeaderLength < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        try {
            packet = new IEEE_802_11Packet(ParentFrame(), PacketStartIndex() + radiotapHeaderLength, PacketEndIndex());
        } catch (...) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + radiotapHeaderLength, PacketEndIndex());
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
