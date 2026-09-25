#include "NetBiosSessionService.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

bool NetBiosSessionService::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result) {
    // Should not be called directly per C# code Exception logic, but we implement basic stub.
    return TryParse(parentFrame, packetStartIndex, packetEndIndex, 0, 0, result);
}

bool NetBiosSessionService::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, AbstractPacket*& result, bool isVirtualPacketFromTrailingDataInTcpSegment) {
    result = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 4 > static_cast<int>(dataLen)) return false;

    bool raw = sourcePort == 445 || destinationPort == 445;
    uint32_t sessionServiceHeader = Utils::ByteConverter::ToUInt32(data, packetStartIndex);

    if (sessionServiceHeader == 0x85000000) {
        result = new NetBiosSessionService(parentFrame, packetStartIndex, packetStartIndex + 3, raw);
        return true;
    } else {
        uint8_t allowedCommands[] = { 0x00, 0x81, 0x82, 0x83, 0x84, 0x85 };
        uint8_t firstByte = static_cast<uint8_t>((sessionServiceHeader >> 24) & 0xFF);
        bool allowed = false;
        for (uint8_t cmd : allowedCommands) {
            if (firstByte == cmd) { allowed = true; break; }
        }
        if (!allowed) return false;

        uint32_t length;
        if (raw) length = sessionServiceHeader & 0x00ffffff;
        else length = sessionServiceHeader & 0x0001ffff;

        if (length == static_cast<uint32_t>(packetEndIndex - packetStartIndex + 1 - 4)) {
            result = new NetBiosSessionService(parentFrame, packetStartIndex, packetEndIndex, raw);
            return true;
        } else if (length < static_cast<uint32_t>(packetEndIndex - packetStartIndex + 1 - 4)) {
            if (packetStartIndex + length + 4 < dataLen) {
                uint8_t nextPacketHeaderByte = data[packetStartIndex + length + 4];
                if (nextPacketHeaderByte == 0x00 || nextPacketHeaderByte == 0x85) {
                    result = new NetBiosSessionService(parentFrame, packetStartIndex, packetStartIndex + length + 3, raw);
                    return true;
                }
            }
            return false;
        } else {
            if (length > 0x1000 && packetEndIndex - packetStartIndex + 1 - 4 < 800 && !isVirtualPacketFromTrailingDataInTcpSegment) {
                if (length == 0xfff7) {
                    const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, QString("EternalBlue exploit attempt, nbss size = 0x%1").arg(length, 4, 16, QChar('0'))));
                }
            }
            return false;
        }
    }
}

NetBiosSessionService::NetBiosSessionService(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool raw)
    : NetBiosPacket(parentFrame, packetStartIndex, packetEndIndex, "NetBIOS Session Service")
{
    const uint8_t* data = parentFrame->Data();
    messageType = data[packetStartIndex];

    if (messageType == 0x85 && packetEndIndex - packetStartIndex == 3) {
        length = 0;
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Message", "NetBios Session Service session keep-alive");
        }
    } else {
        uint32_t l = Utils::ByteConverter::ToUInt32(data, packetStartIndex);
        if (raw) length = static_cast<int>(l & 0x00ffffff);
        else length = static_cast<int>(l & 0x0001ffff);

        if (!ParentFrame()->QuickParse()) {
            addAttribute("Length", QString::number(length));
        }

        if (length > 0 && PacketEndIndex() > PacketStartIndex() + length - 1) {
            setPacketEndIndex(PacketStartIndex() + length - 1); // Note: Need a protected setPacketEndIndex method
        }
    }
}

std::vector<AbstractPacket*> NetBiosSessionService::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (messageType == 0x00 && PacketStartIndex() + 4 < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        // Omit SMB logic for now...
        packet = new RawPacket(ParentFrame(), PacketStartIndex() + 4, PacketEndIndex());
        
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
