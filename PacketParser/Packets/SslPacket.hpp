#pragma once
#include "AbstractPacket.hpp"
// #include "AbstractPacket /*TlsRecordPacket*/.hpp"
#include "RawPacket.hpp"

namespace PacketParser {
namespace Packets {

class SslPacket : public AbstractPacket {
public:
    SslPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
