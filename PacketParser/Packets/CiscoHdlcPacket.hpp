#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class CiscoHdlcPacket : public AbstractPacket {
private:
    uint16_t protocolCode;
    bool hasProtocolCode;

public:
    CiscoHdlcPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
