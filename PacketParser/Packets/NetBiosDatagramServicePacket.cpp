#include "NetBiosDatagramServicePacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

NetBiosDatagramServicePacket::NetBiosDatagramServicePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : NetBiosPacket(parentFrame, packetStartIndex, packetEndIndex, "NetBIOS Datagram Service")
    , smbPacketIndex(-1)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 10 > static_cast<int>(dataLen)) return;

    messageType = data[packetStartIndex];
    flags = Flags(data[packetStartIndex + 1]);
    datagramID = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    sourceIP = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4);
    sourcePort = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 8);

    if (messageType == static_cast<uint8_t>(MessageType::DirectUniqueDatagram) ||
        messageType == static_cast<uint8_t>(MessageType::DirectGroupDatagram) ||
        messageType == static_cast<uint8_t>(MessageType::BroadcastDatagram)) {
        
        if (packetStartIndex + 14 > static_cast<int>(dataLen)) return;

        datagramLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 10);
        packetOffset = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 12);
        
        int index = packetStartIndex + 14;
        sourceName = NetBiosPacket::DecodeNetBiosName(parentFrame, index);
        destinationName = NetBiosPacket::DecodeNetBiosName(parentFrame, index);
        
        if (index + 32 <= packetStartIndex + datagramLength && (messageType == static_cast<uint8_t>(MessageType::DirectUniqueDatagram) || messageType == static_cast<uint8_t>(MessageType::DirectGroupDatagram))) {
            smbPacketIndex = index;
        }
    } else if (messageType == static_cast<uint8_t>(MessageType::DatagramError)) {
        // do nothing
    } else if (messageType == static_cast<uint8_t>(MessageType::DatagramQueryRequest) ||
               messageType == static_cast<uint8_t>(MessageType::DatagramPositiveQueryResponse) ||
               messageType == static_cast<uint8_t>(MessageType::DatagramNegativeQueryResponse)) {
        int index = packetStartIndex + 10;
        destinationName = NetBiosPacket::DecodeNetBiosName(parentFrame, index);
    }
}

std::vector<AbstractPacket*> NetBiosDatagramServicePacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (smbPacketIndex != -1 && smbPacketIndex < PacketEndIndex()) {
        // Omit SMB parsing for now, fall back to RawPacket
        AbstractPacket* smb = new RawPacket(ParentFrame(), smbPacketIndex, PacketEndIndex());
        subPackets.push_back(smb);
        std::vector<AbstractPacket*> childSubPackets = smb->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }

    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
