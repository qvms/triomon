#pragma once
#include "AbstractPacket.hpp"
#include "ITransportLayerPacket.hpp"

namespace PacketParser {
namespace Packets {

class IcmpPacket : public AbstractPacket, public virtual ITransportLayerPacket {
private:
    uint8_t m_type;
    uint8_t m_code;
    uint16_t m_checksum;

    static uint8_t GetReverseType(uint8_t type, uint8_t code, bool& unidirectional);

public:
    static bool IsUnidirectionalType(uint8_t type);

    IcmpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t SourcePort() const override { return m_type; }
    uint16_t DestinationPort() const override { 
        bool uni = false;
        return GetReverseType(m_type, m_code, uni);
    }
    uint8_t DataOffsetByteCount() const override { return 8; }
    uint8_t FlagsRaw() const override { return m_type; }
    uint16_t Checksum() const override { return m_checksum; }
    RFC1700Protocol TransportProtocol() const override { return RFC1700Protocol::ICMP; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
