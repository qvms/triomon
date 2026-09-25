#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"

namespace PacketParser {
namespace Packets {

class OpenFlowPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    enum class Version : uint8_t { v1_0 = 1, v1_1 = 2, v1_2 = 3, v1_3 = 4, v1_4 = 5 };

    enum class OfpType : uint8_t {
        OFPT_HELLO = 0, OFPT_ERROR = 1, OFPT_ECHO_REQUEST = 2, OFPT_ECHO_REPLY = 3, OFPT_EXPERIMENTER = 4,
        OFPT_FEATURES_REQUEST = 5, OFPT_FEATURES_REPLY = 6, OFPT_GET_CONFIG_REQUEST = 7, OFPT_GET_CONFIG_REPLY = 8,
        OFPT_SET_CONFIG = 9, OFPT_PACKET_IN = 10, OFPT_FLOW_REMOVED = 11, OFPT_PORT_STATUS = 12,
        OFPT_PACKET_OUT = 13, OFPT_FLOW_MOD = 14, OFPT_GROUP_MOD = 15, OFPT_PORT_MOD = 16, OFPT_TABLE_MOD = 17,
        OFPT_MULTIPART_REQUEST = 18, OFPT_MULTIPART_REPLY = 19, OFPT_BARRIER_REQUEST = 20, OFPT_BARRIER_REPLY = 21,
        OFPT_QUEUE_GET_CONFIG_REQUEST = 22, OFPT_QUEUE_GET_CONFIG_REPLY = 23, OFPT_ROLE_REQUEST = 24,
        OFPT_ROLE_REPLY = 25, OFPT_GET_ASYNC_REQUEST = 26, OFPT_GET_ASYNC_REPLY = 27, OFPT_SET_ASYNC = 28,
        OFPT_METER_MOD = 29
    };

private:
    uint8_t m_version;
    uint8_t m_type;
    uint16_t m_length;
    int m_parsedBytesCount;
    int m_nextPacketIndex;

    OpenFlowPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    int getPadding(int structLength) const;

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override;
    int ParsedBytesCount() const override { return m_parsedBytesCount; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
