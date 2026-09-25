#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class Mpls : public AbstractPacket {
private:
    static const int PAYLOAD_OFFSET = 4;
    bool bottomOfStack;
    uint32_t label;

public:
    Mpls(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
