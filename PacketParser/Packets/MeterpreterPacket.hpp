#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

class MeterpreterPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    uint32_t PayloadLength;
    bool HasMZHeader;

    MeterpreterPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override { return true; }
    int ParsedBytesCount() const override {
        return PayloadLength == 0 ? 0 : 4;
    }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
