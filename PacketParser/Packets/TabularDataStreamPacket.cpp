#include "TabularDataStreamPacket.hpp"
#include "../Utils.hpp"
#include <algorithm>

namespace PacketParser {
namespace Packets {

TabularDataStreamPacket::TabularDataStreamPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Tabular Data Stream (SQL)")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 2 >= static_cast<int>(dataLen)) {
        return; // Truncated
    }

    packetType = data[packetStartIndex];
    isLastPacket = (data[packetStartIndex + 1] == 0x01);
    packetSize = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);

    int subPacketIndex = packetStartIndex + 4 + 4; // skip 4 bytes of something

    if (packetType == static_cast<uint8_t>(PacketTypes::SqlQuery)) {
        if (subPacketIndex < static_cast<int>(dataLen)) {
            int length = std::min(packetEndIndex - subPacketIndex + 1, static_cast<int>(packetSize) - 8);
            if (length > 0 && subPacketIndex + length <= static_cast<int>(dataLen)) {
                query = Utils::ByteConverter::ReadStringOriginal(data, subPacketIndex, length, true, true);
            }
        }
    }
    else if (packetType == static_cast<uint8_t>(PacketTypes::Tds7Login)) {
        if (subPacketIndex + 70 <= static_cast<int>(dataLen)) {
            clientHostname = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 36, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 38, true),
                true, true);
            
            username = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 40, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 42, true),
                true, true);
                
            int tmp = subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 44, true);
            password = Utils::ByteConverter::ReadStringAndAdvance(
                data,
                tmp,
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 46, true),
                true, true, Utils::ByteConverter::Encoding::TDS_password);
                
            appname = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 48, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 50, true),
                true, true);
                
            serverHostname = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 52, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 54, true),
                true, true);
                
            libraryName = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 60, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 62, true),
                true, true);
                
            databaseName = Utils::ByteConverter::ReadString(
                data,
                subPacketIndex + Utils::ByteConverter::ToUInt16(data, subPacketIndex + 68, true),
                2 * Utils::ByteConverter::ToUInt16(data, subPacketIndex + 70, true),
                true, true);

            if (!parentFrame->QuickParse()) {
                if (!clientHostname.isEmpty()) addAttribute("SQL client hostname", clientHostname);
                if (!username.isEmpty()) addAttribute("SQL username", username);
                if (!password.isEmpty()) addAttribute("SQL password", password);
                if (!appname.isEmpty()) addAttribute("App name", appname);
                if (!serverHostname.isEmpty()) addAttribute("SQL server", serverHostname);
                if (!libraryName.isEmpty()) addAttribute("SQL library", libraryName);
                if (!databaseName.isEmpty()) addAttribute("Database name", databaseName);
            }
        }
    }
}

std::vector<AbstractPacket*> TabularDataStreamPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
