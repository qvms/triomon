#include "NullLoopbackPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

NullLoopbackPacket::NullLoopbackPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Null/Loopback")
    , protocolFamily(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    protocolFamily = Utils::ByteConverter::ToUInt32(data, packetStartIndex, true);
}

std::vector<AbstractPacket*> NullLoopbackPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + PACKET_LENGTH <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;

        if (protocolFamily == static_cast<uint32_t>(ProtocolFamily::AF_INET)) {
            if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + PACKET_LENGTH, PacketEndIndex(), packet)) {
                packet = nullptr;
            }
        } else if (protocolFamily == static_cast<uint32_t>(ProtocolFamily::AF_INET6_OpenBSD) ||
                   protocolFamily == static_cast<uint32_t>(ProtocolFamily::AF_INET6_FreeBSD) ||
                   protocolFamily == static_cast<uint32_t>(ProtocolFamily::AF_INET6_OSX)) {
            packet = new IPv6Packet(ParentFrame(), PacketStartIndex() + PACKET_LENGTH, PacketEndIndex());
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
