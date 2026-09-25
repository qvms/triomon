#pragma once
#include "AbstractPacket.hpp"
#include <cstdint>
#include <vector>

namespace PacketParser {
namespace Packets {

class UdpPacket : public AbstractPacket {
private:
    uint16_t m_sourcePort;
    uint16_t m_destinationPort;
    uint16_t m_length;
    uint16_t m_checksum;

public:
    UdpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t SourcePort() const { return m_sourcePort; }
    uint16_t DestinationPort() const { return m_destinationPort; }
    uint8_t DataOffsetByteCount() const { return 8; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
