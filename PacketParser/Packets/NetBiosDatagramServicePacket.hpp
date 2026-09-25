#pragma once
#include "NetBiosPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class NetBiosDatagramServicePacket : public NetBiosPacket {
public:
    class Flags {
    public:
        enum class SourceEndNodeTypeEnum : uint8_t { B = 0, P = 1, M = 2, NBDD = 3 };

    private:
        uint8_t flagData;

    public:
        Flags(uint8_t value) : flagData(value) {}
        Flags() : flagData(0) {}

        SourceEndNodeTypeEnum SourceEndNodeType() const { return static_cast<SourceEndNodeTypeEnum>((flagData >> 2) & 0x03); }
        bool ThisIsFirstFragment() const { return (flagData & 0x02) == 0x02; }
        bool MoreDatagramFragmentsFollow() const { return (flagData & 0x01) == 0x01; }
    };

    enum class MessageType : uint8_t {
        DirectUniqueDatagram = 0x10,
        DirectGroupDatagram = 0x11,
        BroadcastDatagram = 0x12,
        DatagramError = 0x13,
        DatagramQueryRequest = 0x14,
        DatagramPositiveQueryResponse = 0x15,
        DatagramNegativeQueryResponse = 0x16
    };

private:
    uint8_t messageType;
    Flags flags;
    uint16_t datagramID;
    uint32_t sourceIP;
    uint16_t sourcePort;
    uint16_t datagramLength;
    uint16_t packetOffset;
    int smbPacketIndex; // -1 for null

    QString sourceName;
    QString destinationName;

public:
    NetBiosDatagramServicePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    QString SourceNetBiosName() const { return sourceName; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
