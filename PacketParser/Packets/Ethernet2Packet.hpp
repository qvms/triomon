#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <cstdint>

namespace PacketParser {
namespace Packets {

class Ethernet2Packet : public AbstractPacket {
public:
    enum class EtherTypes : uint16_t {
        IEEE802_3_Max = 0x0600,
        HPSW =          0x0623,
        IPv4 =          0x0800,
        ARP =           0x0806,
        TransparentEthernet = 0x6558,
        IEEE802_1Q =    0x8100,
        xHayesTunnel =  0x8130,
        IPv6 =          0x86dd,
        MPLS =          0x8847,
        PPPoE =         0x8864,
        ERSPAN =        0x88be
    };

private:
    QString m_sourceMAC;
    QString m_destinationMAC;
    uint16_t m_etherType;

public:
    static bool TryGetPacketForType(uint16_t etherType, const Frame* parentFrame, int newPacketStartIndex, int newPacketEndIndex, AbstractPacket*& packet);
    static AbstractPacket* GetPacketForType(uint16_t etherType, const Frame* parentFrame, int newPacketStartIndex, int newPacketEndIndex);

    Ethernet2Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    QString SourceMACAddress() const { return m_sourceMAC; }
    QString DestinationMACAddress() const { return m_destinationMAC; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

private:
    QString ConvertToHexString(const uint8_t* data, size_t len);
};

} // namespace Packets
} // namespace PacketParser
