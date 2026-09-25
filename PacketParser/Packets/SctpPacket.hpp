#pragma once
#include "AbstractPacket.hpp"
#include "ITransportLayerPacket.hpp"

namespace PacketParser {
namespace Packets {

class SctpPacket : public AbstractPacket, public virtual ITransportLayerPacket {
private:
    uint16_t m_sourcePort;
    uint16_t m_destinationPort;
    uint32_t m_verificationTag;
    uint32_t m_checksum;

public:
    SctpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t SourcePort() const override { return m_sourcePort; }
    uint16_t DestinationPort() const override { return m_destinationPort; }
    uint32_t VerificationTag() const { return m_verificationTag; }
    uint16_t Checksum() const override { return static_cast<uint16_t>(m_checksum); }
    uint32_t ChecksumFull() const { return m_checksum; }
    uint8_t DataOffsetByteCount() const override { return 28; }
    uint8_t FlagsRaw() const override { return 0; }
    RFC1700Protocol TransportProtocol() const override { return RFC1700Protocol::SCTP; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
