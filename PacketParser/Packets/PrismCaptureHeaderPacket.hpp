#pragma once
#include "AbstractPacket.hpp"
#include <vector>
#include <QString>

namespace PacketParser {
namespace Packets {

class PrismCaptureHeaderPacket : public AbstractPacket {
private:
    uint32_t messageLength;
    QString device;
    uint32_t channel;

public:
    PrismCaptureHeaderPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint32_t MessageLength() const { return messageLength; }
    QString Device() const { return device; }
    uint32_t Channel() const { return channel; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
