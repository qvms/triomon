#pragma once
#include "AbstractPacket.hpp"
#include "../Utils.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

class SyslogPacket : public AbstractPacket {
public:
    QString SyslogMessage;

    SyslogPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
