#include "PointToPointOverEthernetPacket.hpp"
#include "PointToPointPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

const int PAYLOAD_OFFSET = 6;

enum class PPPoECODE : uint8_t {
    SessionData = 0x00,
    ActiveDiscoveryOffer = 0x07,
    ActiveDiscoveryInitiation = 0x09,
    ActiveDiscoveryRequest = 0x19,
    ActiveDiscoverySessionConfirmation = 0x65,
    ActiveDiscoveryTerminate = 0xa7
};

PointToPointOverEthernetPacket::PointToPointOverEthernetPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Point-to-point protocol over Ethernet (PPPoE)")
    , payloadLength(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();
    
    if (packetStartIndex + 4 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    if (data[packetStartIndex] != 0x11) {
        // throw new Exception("Invalid PPPoE Version or Type");
        return;
    }
    
    code = data[packetStartIndex + 1];
    
    if (code == static_cast<uint8_t>(PPPoECODE::SessionData)) {
        payloadLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);
    }
}

std::vector<AbstractPacket*> PointToPointOverEthernetPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (payloadLength > 0 && code == static_cast<uint8_t>(PPPoECODE::SessionData)) {
        int pppEndIndex = PacketStartIndex() + PAYLOAD_OFFSET + payloadLength - 1;
        if (pppEndIndex <= PacketEndIndex()) {
            PointToPointPacket* packet = new PointToPointPacket(ParentFrame(), PacketStartIndex() + PAYLOAD_OFFSET, pppEndIndex);
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
