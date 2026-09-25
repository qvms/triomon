#include "IEEE_802_11ProbeRequestBodyPacket.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"
#include <stdexcept>

namespace PacketParser {
namespace Packets {

IEEE_802_11ProbeRequestBodyPacket::IEEE_802_11ProbeRequestBodyPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IEEE 802.11 Probe Request Body") {
    
    if (packetStartIndex + 1 < parentFrame->GetData().size()) {
        uint8_t length = static_cast<uint8_t>(parentFrame->GetData()[packetStartIndex + 1]);
        if (packetStartIndex + 2 + length <= parentFrame->GetData().size()) {
            QByteArray ba = parentFrame->GetData().mid(packetStartIndex + 2, length);
            requestedSsid = PacketParser::Utils::ByteConverter::ReadString(ba);
        }
    }
}

std::vector<AbstractPacket*> IEEE_802_11ProbeRequestBodyPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
