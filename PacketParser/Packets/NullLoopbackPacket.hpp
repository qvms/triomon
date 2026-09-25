#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class NullLoopbackPacket : public AbstractPacket {
public:
    enum class ProtocolFamily : uint32_t {
        AF_INET = 2,
        AF_INET6_OpenBSD = 24,
        AF_INET6_FreeBSD = 28,
        AF_INET6_OSX = 30
    };

private:
    static const int PACKET_LENGTH = 4;
    uint32_t protocolFamily;

public:
    NullLoopbackPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
