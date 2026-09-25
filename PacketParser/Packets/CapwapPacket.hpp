#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class CapwapPacket : public AbstractPacket {
public:
    enum class CapwapType : uint8_t {
        Capwap = 0,
        DTLS = 1
    };
    enum class WirelessBindingID : uint8_t {
        Reserved = 0,
        IEEE_802_11 = 1,
        Reserved2 = 2,
        EPCGlobal = 3
    };

private:
    uint8_t headerVersion;
    uint8_t headerType;
    uint8_t wirelessBindingID;
    int headerLength;
    bool payloadIsWBID;

public:
    CapwapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
