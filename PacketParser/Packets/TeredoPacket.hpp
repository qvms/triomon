#pragma once
#include "AbstractPacket.hpp"
#include "IPv6Packet.hpp"

namespace PacketParser {
namespace Packets {

class TeredoPacket : public AbstractPacket {
private:
    IPv6Packet* m_ipv6Packet;

public:
    TeredoPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    virtual ~TeredoPacket() { delete m_ipv6Packet; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
