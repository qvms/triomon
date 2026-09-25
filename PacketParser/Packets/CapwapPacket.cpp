#include "CapwapPacket.hpp"
#include "IEEE_802_11Packet.hpp"
#include "RawPacket.hpp"

namespace PacketParser {
namespace Packets {

CapwapPacket::CapwapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "CAPWAP")
    , headerVersion(0)
    , headerType(0)
    , wirelessBindingID(0)
    , headerLength(0)
    , payloadIsWBID(false)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex >= static_cast<int>(dataLen)) {
        return;
    }

    headerVersion = static_cast<uint8_t>(data[packetStartIndex] >> 4);
    headerType = static_cast<uint8_t>(data[packetStartIndex] & 0x0F);

    if (headerType == static_cast<uint8_t>(CapwapType::DTLS)) {
        headerLength = 4;
    } else if (headerType == static_cast<uint8_t>(CapwapType::Capwap)) {
        if (packetStartIndex + 2 < static_cast<int>(dataLen)) {
            headerLength = 4 * (data[packetStartIndex + 1] >> 3);
            wirelessBindingID = static_cast<uint8_t>((data[packetStartIndex + 2] >> 1) & 0x1F);
            payloadIsWBID = (data[packetStartIndex + 2] & 0x01) == 0x01;
        }
    }
}

std::vector<AbstractPacket*> CapwapPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (headerType == static_cast<uint8_t>(CapwapType::Capwap)) {
        if (payloadIsWBID) {
            if (wirelessBindingID == static_cast<uint8_t>(WirelessBindingID::IEEE_802_11)) {
                if (PacketStartIndex() + headerLength <= PacketEndIndex()) {
                    IEEE_802_11Packet* iee80211 = new IEEE_802_11Packet(ParentFrame(), PacketStartIndex() + headerLength, PacketEndIndex(), true);
                    subPackets.push_back(iee80211);
                    std::vector<AbstractPacket*> childSubPackets = iee80211->GetSubPackets(false);
                    subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
                }
            }
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
