#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class ErfFrame : public AbstractPacket {
public:
    enum class RecordTypes : uint8_t {
        ERF_TYPE_LEGACY = 0,
        ERF_TYPE_HDLC_POS = 1,
        ERF_TYPE_ETH = 2,
        ERF_TYPE_ATM = 3,
        ERF_TYPE_AAL5 = 4,
        ERF_TYPE_MC_HDLC = 5,
        ERF_TYPE_MC_RAW = 6,
        ERF_TYPE_MC_ATM = 7,
        ERF_TYPE_MC_RAW_CHANNEL = 8,
        ERF_TYPE_MC_AAL5 = 9,
        ERF_TYPE_COLOR_HDLC_POS = 10,
        ERF_TYPE_COLOR_ETH = 11,
        ERF_TYPE_MC_AAL2 = 12,
        ERF_TYPE_IP_COUNTER = 13,
        ERF_TYPE_TCP_FLOW_COUNTER = 14,
        ERF_TYPE_DSM_COLOR_HDLC_POS = 15,
        ERF_TYPE_DSM_COLOR_ETH = 16,
        ERF_TYPE_COLOR_MC_HDLC_POS = 17,
        ERF_TYPE_AAL2 = 18,
        ERF_TYPE_INFINIBAND = 21,
        ERF_TYPE_IPV4 = 22,
        ERF_TYPE_IPV6 = 23,
        ERF_TYPE_RAW_LINK = 24,
        ERF_TYPE_INFINIBAND_LINK = 25
    };

private:
    uint8_t type;
    bool extensionHeadersPresent;

public:
    ErfFrame(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
