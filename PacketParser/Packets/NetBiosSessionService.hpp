#pragma once
#include "NetBiosPacket.hpp"
#include "ISessionPacket.hpp"

namespace PacketParser {
namespace Packets {

class NetBiosSessionService : public NetBiosPacket, public ISessionPacket {
private:
    uint8_t messageType;
    int length;

    NetBiosSessionService(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool raw);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result);
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, AbstractPacket*& result, bool isVirtualPacketFromTrailingDataInTcpSegment = false);

    uint8_t MessageType() const { return messageType; }
    int Length() const { return length; }

    bool PacketHeaderIsComplete() const override { return PacketLength() >= 4; }
    int ParsedBytesCount() const override {
        if (PacketLength() >= length + 4) return length + 4;
        return 0;
    }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
