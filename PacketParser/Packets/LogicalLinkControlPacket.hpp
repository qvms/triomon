#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class LogicalLinkControlPacket : public AbstractPacket {
public:
    enum class ServiceAccessPointType : uint8_t {
        NullLsap = 0x00,
        SpanningTree = 0x42,
        X_25overIEEE802_2 = 0x7e,
        SubNetworkAccessProtocol = 0xaa,
        IbmNetBIOS = 0xf0,
        HpExtendedLLC = 0xf8,
        ISONetworkLayerProtocol = 0xfe
    };

private:
    uint8_t dsap;
    uint8_t ssap;
    uint8_t control;
    uint32_t organisationCode;
    uint16_t etherType;

public:
    LogicalLinkControlPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
