#include "GrePacket.hpp"
#include "Ethernet2Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

GrePacket::GrePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "GRE")
    , greHeaderLength(4)
    , etherType(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    uint16_t flagsAndVersion = Utils::ByteConverter::ToUInt16(data, packetStartIndex, false);
    
    if ((flagsAndVersion & static_cast<uint16_t>(Flags::ChecksumPresent)) != 0 || 
        (flagsAndVersion & static_cast<uint16_t>(Flags::RoutingPresent)) != 0) {
        greHeaderLength += 4;
    }
    if ((flagsAndVersion & static_cast<uint16_t>(Flags::KeyPresent)) != 0) {
        greHeaderLength += 4;
    }
    if ((flagsAndVersion & static_cast<uint16_t>(Flags::SequenceNumberPresent)) != 0) {
        greHeaderLength += 4;
    }
    
    etherType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, false);
}

std::vector<AbstractPacket*> GrePacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + greHeaderLength <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        if (Ethernet2Packet::TryGetPacketForType(etherType, ParentFrame(), PacketStartIndex() + greHeaderLength, PacketEndIndex(), packet)) {
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
