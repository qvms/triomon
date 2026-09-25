#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class TzspPacket : public AbstractPacket {
public:
    enum class TzspEncapsulation : uint16_t {
        Ethernet = 1,
        TokenRing = 2,
        SLIP = 3,
        PPP = 4,
        FDDI = 5,
        RAW = 7,
        IEEE_802_11 = 18,
        IEEE_802_11_PRISM = 119,
        IEEE_802_11_RADIOTAP = 126,
        IEEE_802_11_AVS = 127,
    };

private:
    enum class TzspTag : uint8_t {
        Padding = 0,
        End = 1
    };

    uint8_t m_version;
    uint8_t m_type;
    uint16_t m_encapsulation;
    int encapsulatedPacketStartIndex;

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& tzspPacket);

    TzspPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint8_t Version() const { return m_version; }
    uint8_t Type() const { return m_type; }
    uint16_t Encapsulation() const { return m_encapsulation; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
