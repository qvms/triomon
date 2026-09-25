#pragma once
#include "AbstractPacket.hpp"
#include "Ethernet2Packet.hpp"
#include <optional>

namespace PacketParser {
namespace Packets {

class VxlanPacket : public AbstractPacket {
private:
    std::optional<int> m_vxlanNetworkIdentifier;
    Ethernet2Packet* m_innerEthernetPacket;

public:
    VxlanPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    virtual ~VxlanPacket() { delete m_innerEthernetPacket; }

    std::optional<int> VxlanNetworkIdentifier() const { return m_vxlanNetworkIdentifier; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
