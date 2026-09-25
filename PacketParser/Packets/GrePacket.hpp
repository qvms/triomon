#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class GrePacket : public AbstractPacket {
public:
    enum class Flags : uint16_t {
        ChecksumPresent = 1 << 15,
        RoutingPresent = 1 << 14,
        KeyPresent = 1 << 13,
        SequenceNumberPresent = 1 << 12,
        StrictSourceRoute = 1 << 11
    };

private:
    int greHeaderLength;
    uint16_t etherType;

public:
    GrePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
