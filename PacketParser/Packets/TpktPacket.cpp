#include "TpktPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

bool TpktPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const TcpPacket* parentTcpPacket, AbstractPacket*& result) {
    result = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 > packetEndIndex || packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        return false;
    }
    if (data[packetStartIndex] != 3) {
        return false;
    }
    if (data[packetStartIndex + 1] != 0) {
        return false;
    }

    uint16_t length = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);

    if (length > packetEndIndex - packetStartIndex + 1 || length < 4) {
        return false;
    }

    try {
        result = new TpktPacket(parentFrame, packetStartIndex, packetEndIndex, parentTcpPacket);
        return true;
    } catch (...) {
        return false;
    }
}

TpktPacket::TpktPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const TcpPacket* parentTcpPacket)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TPKT")
    , m_parentTcpPacket(parentTcpPacket)
{
    const uint8_t* data = parentFrame->Data();
    if (static_cast<size_t>(packetStartIndex + 2) < parentFrame->DataLength()) {
        m_version = data[packetStartIndex];
        m_length = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    } else {
        m_version = 0;
        m_length = 0;
    }
}

std::vector<AbstractPacket*> TpktPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketLength() >= m_length) {
        AbstractPacket* packet = nullptr;

        if (m_parentTcpPacket->DestinationPort() == 3389 || m_parentTcpPacket->SourcePort() == 3389) {
            // COTP parsing omitted for now. Fall back to Raw.
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + 4, PacketStartIndex() + m_length - 1);
        } else if (m_parentTcpPacket->DestinationPort() == 102 || m_parentTcpPacket->SourcePort() == 102) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + 4, PacketStartIndex() + m_length - 1);
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
