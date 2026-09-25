#pragma once
#include "AbstractPacket.hpp"
#include "IIPPacket.hpp"
#include <QHostAddress>
#include <vector>

namespace PacketParser {
namespace Packets {

class HopByHopOption : public AbstractPacket {
public:
    const uint8_t NextHeaderRFC1700Protocol;

private:
    int m_nextHeaderStartOffset;

public:
    HopByHopOption(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

class IPv6Packet : public AbstractPacket, public virtual IIPPacket {
private:
    static const uint8_t IPV6_HEADER_LENGTH = 40;

    uint16_t m_payloadLength;
    uint8_t m_nextHeader;
    uint8_t m_hopLimit;
    QHostAddress m_sourceIP;
    QHostAddress m_destinationIP;

public:
    IPv6Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    QString SourceIPAddress() const override { return m_sourceIP.toString(); }
    QString DestinationIPAddress() const override { return m_destinationIP.toString(); }
    int PayloadLength() const override { return m_payloadLength; }
    uint8_t HopLimit() const override { return m_hopLimit; }
    uint8_t HeaderLength() const override { return IPV6_HEADER_LENGTH; }
    uint8_t NextRFC1700Protocol() const override { return m_nextHeader; }
    

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
