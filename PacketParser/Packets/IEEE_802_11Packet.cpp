#include "IEEE_802_11Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"
#include <QStringList>

#if __has_include("LogicalLinkControlPacket.hpp")
#include "LogicalLinkControlPacket.hpp"
#endif

namespace PacketParser {
namespace Packets {

IEEE_802_11Packet::IEEE_802_11Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool swapFrameControlBytes)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IEEE 802.11")
    , frameControl(nullptr)
    , duration(0)
    , fragmentNibble(0)
    , sequenceNumber(0)
    , dataOffsetByteCount(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 1 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    if (swapFrameControlBytes) {
        frameControl = new FrameControl(data[packetStartIndex + 1], data[packetStartIndex]);
    } else {
        frameControl = new FrameControl(data[packetStartIndex], data[packetStartIndex + 1]);
    }

    if (!parentFrame->QuickParse()) {
        addAttribute("Type", QString::number(frameControl->Type()));
        addAttribute("Sub Type", QString::number(frameControl->SubType()));
        addAttribute("ToDS", frameControl->ToDistributionSystem() ? "True" : "False");
        addAttribute("FromDS", frameControl->FromDistributionSystem() ? "True" : "False");
        addAttribute("WEP", frameControl->WEP() ? "True" : "False");
    }

    if (packetStartIndex + 3 < static_cast<int>(dataLen)) {
        duration = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, true);
        if (!parentFrame->QuickParse()) {
            addAttribute("Duration", QString::number(duration));
        }
    }

    int index = packetStartIndex + 4;
    std::vector<QString> addresses(4);

    for (int i = 0; i < 4; i++) {
        if (i < 3) {
            if (index <= packetEndIndex - 5 && index + 5 < static_cast<int>(dataLen)) {
                addresses[i] = ConvertToHexString(data + index, 6);
                index += 6;
            } else {
                addresses[i] = QString();
            }
        }
    }

    if (frameControl->Type() == 0 || frameControl->Type() == 2) {
        if (index < static_cast<int>(dataLen)) {
            fragmentNibble = data[index] & 0x0F;
            if (index + 1 < static_cast<int>(dataLen)) {
                sequenceNumber = static_cast<uint16_t>(Utils::ByteConverter::ToUInt16(data, index, true) >> 4);
            }
        }
        index += 2;

        if (frameControl->FromDistributionSystem() && frameControl->ToDistributionSystem()) {
            if (index <= packetEndIndex - 5 && index + 5 < static_cast<int>(dataLen)) {
                addresses[3] = ConvertToHexString(data + index, 6);
                index += 6;
            }
        }
    }

    if (frameControl->Type() == 2 && frameControl->SubType() >= 8) {
        index += 2;
    }

    dataOffsetByteCount = index - packetStartIndex;

    if (frameControl->Type() == 0 || frameControl->Type() == 2) {
        if (!frameControl->ToDistributionSystem() && !frameControl->FromDistributionSystem()) {
            destinationMAC = addresses[0];
            sourceMAC = addresses[1];
            basicServiceSetMAC = addresses[2];
        } else if (!frameControl->ToDistributionSystem() && frameControl->FromDistributionSystem()) {
            destinationMAC = addresses[0];
            basicServiceSetMAC = addresses[1];
            sourceMAC = addresses[2];
        } else if (frameControl->ToDistributionSystem() && !frameControl->FromDistributionSystem()) {
            basicServiceSetMAC = addresses[0];
            sourceMAC = addresses[1];
            destinationMAC = addresses[2];
        } else if (frameControl->ToDistributionSystem() && frameControl->ToDistributionSystem()) { // 1,1
            recipientMAC = addresses[0];
            transmitterMAC = addresses[1];
            destinationMAC = addresses[2];
            sourceMAC = addresses[3];
        }
    } else if (frameControl->Type() == 1) {
        recipientMAC = addresses[0];
        transmitterMAC = addresses[1];
    }

    if (!parentFrame->QuickParse()) {
        if (!sourceMAC.isEmpty()) addAttribute("Source MAC", sourceMAC);
        if (!destinationMAC.isEmpty()) addAttribute("Destination MAC", destinationMAC);
        if (!transmitterMAC.isEmpty()) addAttribute("Transmitter MAC", transmitterMAC);
        if (!recipientMAC.isEmpty()) addAttribute("Recipient MAC", recipientMAC);
        if (!basicServiceSetMAC.isEmpty()) addAttribute("BSSID", basicServiceSetMAC);
    }
}

IEEE_802_11Packet::~IEEE_802_11Packet() {
    delete frameControl;
}

QString IEEE_802_11Packet::ConvertToHexString(const uint8_t* data, size_t len) {
    QStringList parts;
    for (size_t i = 0; i < len; ++i) {
        parts << QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return parts.join("-");
}

std::vector<AbstractPacket*> IEEE_802_11Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (!frameControl) return subPackets;

    int crcLength = 0;

    if (PacketStartIndex() + dataOffsetByteCount < PacketEndIndex() - crcLength) {
        AbstractPacket* packet = nullptr;

        try {
            if (frameControl->Type() == 2) {
#if __has_include("LogicalLinkControlPacket.hpp")
                packet = new LogicalLinkControlPacket(ParentFrame(), PacketStartIndex() + dataOffsetByteCount, PacketEndIndex() - crcLength);
#else
                packet = new RawPacket(ParentFrame(), PacketStartIndex() + dataOffsetByteCount, PacketEndIndex() - crcLength);
#endif
            } else if (frameControl->Type() == 0) {
                packet = new RawPacket(ParentFrame(), PacketStartIndex() + dataOffsetByteCount, PacketEndIndex() - crcLength);
            } else {
                packet = new RawPacket(ParentFrame(), PacketStartIndex() + dataOffsetByteCount, PacketEndIndex() - crcLength);
            }
        } catch (...) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + dataOffsetByteCount, PacketEndIndex() - crcLength);
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
