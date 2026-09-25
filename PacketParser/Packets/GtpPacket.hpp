#pragma once
#include "AbstractPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include <optional>

namespace PacketParser {
namespace Packets {

class GtpPacket : public AbstractPacket {
private:
    uint8_t m_flagData;
    uint8_t m_version;
    uint8_t m_messageType;
    uint16_t m_messageLength;
    uint32_t m_tunnelEndpointID;
    
    std::optional<std::pair<int, int>> m_tunneledPacketInfo;
    
    bool HasFlagV1(uint8_t flag) const { return m_version == 1 && (m_flagData & flag) != 0; }
    bool HasFlagV2(uint8_t flag) const { return m_version == 2 && (m_flagData & flag) != 0; }

public:
    GtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    virtual ~GtpPacket() = default;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
