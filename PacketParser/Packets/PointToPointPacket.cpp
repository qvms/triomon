#include "PointToPointPacket.hpp"
#include "IPv4Packet.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

const uint8_t ALL_STATIONS_ADDRESS = 0xff;
const uint8_t UNNUMBERED_INFORMATION_COMMAND = 0x03;
const uint16_t IP_PROTOCOL_ID = 0x0021; 

PointToPointPacket::PointToPointPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Point-to-Point Protocol (PPP)")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();
    
    if (packetStartIndex >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    if (data[packetStartIndex] == ALL_STATIONS_ADDRESS) {
        // PPP in HDLC-like Framing
        if (packetStartIndex + 3 > packetEndIndex || packetStartIndex + 3 >= static_cast<int>(dataLen)) {
            return;
        }
        
        uint8_t address = data[packetStartIndex];
        uint8_t control = data[packetStartIndex + 1];

        if (address != ALL_STATIONS_ADDRESS || control != UNNUMBERED_INFORMATION_COMMAND) {
            // throw new Exception("Invalid PPP HDLC framing");
            return;
        }

        protocol = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, false);
        protocolStartOffset = 4;
        
        if (!parentFrame->QuickParse()) {
            addAttribute("Encapsulated protocol", QString("0x%1").arg(protocol, 4, 16, QChar('0')).toUpper());
        }
    } else {
        // Normal Point-to-Point Protocol (PPP)
        if (packetStartIndex + 1 > packetEndIndex || packetStartIndex + 1 >= static_cast<int>(dataLen)) {
            return;
        }
        protocol = Utils::ByteConverter::ToUInt16(data, packetStartIndex, false);
        protocolStartOffset = 2;
    }
}

std::vector<AbstractPacket*> PointToPointPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    if (PacketStartIndex() + protocolStartOffset < PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
        
        if (protocol == IP_PROTOCOL_ID) {
            if (!IPv4Packet::TryParse(ParentFrame(), PacketStartIndex() + protocolStartOffset, PacketEndIndex(), packet)) {
                packet = nullptr;
            }
        }
        
        if (!packet) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + protocolStartOffset, PacketEndIndex());
        }
        
        subPackets.push_back(packet);
        std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
