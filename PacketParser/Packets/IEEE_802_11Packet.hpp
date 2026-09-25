#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <cstdint>

namespace PacketParser {
namespace Packets {

class IEEE_802_11Packet : public AbstractPacket {
public:
    class FrameControl {
    private:
        uint8_t versionTypeSubtype;
        uint8_t flagData;
    public:
        FrameControl(uint8_t firstByte, uint8_t secondByte)
            : versionTypeSubtype(firstByte), flagData(secondByte) {}

        uint8_t ProtocolVersion() const { return (versionTypeSubtype & 0x03); }
        uint8_t Type() const { return (versionTypeSubtype & 0x0C) >> 2; }
        uint8_t SubType() const { return (versionTypeSubtype & 0xF0) >> 4; }

        bool ToDistributionSystem() const { return (flagData & 0x01) == 0x01; }
        bool FromDistributionSystem() const { return (flagData & 0x02) == 0x02; }
        bool MoreFragmentFlag() const { return (flagData & 0x04) == 0x04; }
        bool Retry() const { return (flagData & 0x08) == 0x08; }
        bool PowerManagement() const { return (flagData & 0x10) == 0x10; }
        bool MoreData() const { return (flagData & 0x20) == 0x20; }
        bool WEP() const { return (flagData & 0x40) == 0x40; }
        bool Order() const { return (flagData & 0x80) == 0x80; }
    };

private:
    FrameControl* frameControl;
    uint16_t duration;
    QString sourceMAC;
    QString destinationMAC;
    QString transmitterMAC;
    QString recipientMAC;
    QString basicServiceSetMAC;
    
    uint8_t fragmentNibble;
    uint16_t sequenceNumber;
    int dataOffsetByteCount;

    QString ConvertToHexString(const uint8_t* data, size_t len);

public:
    IEEE_802_11Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool swapFrameControlBytes = false);
    ~IEEE_802_11Packet() override;

    QString SourceMAC() const { return sourceMAC; }
    QString DestinationMAC() const { return destinationMAC; }
    QString BSSID() const { return basicServiceSetMAC; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
