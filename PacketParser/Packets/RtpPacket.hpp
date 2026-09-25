#pragma once
#include "AbstractPacket.hpp"

namespace PacketParser {
namespace Packets {

class RtpPacket : public AbstractPacket {
public:
    enum class RTPVersion : uint8_t { VAT = 0, Draft = 1, RFC1889 = 2, v3 = 3 };
    static const int HEADER_LENGTH = 12;

private:
    RTPVersion m_rtpVersion;
    bool m_padding;
    bool m_extension;
    uint8_t m_csrcCount;
    bool m_marker;
    uint8_t m_payloadType;
    uint16_t m_sequenceNumber;
    uint32_t m_sampleTick;
    uint32_t m_syncSourceID;

public:
    RtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    RTPVersion Version() const { return m_rtpVersion; }
    bool Padding() const { return m_padding; }
    bool Extension() const { return m_extension; }
    uint8_t CsrcCount() const { return m_csrcCount; }
    bool Marker() const { return m_marker; }
    uint8_t PayloadType() const { return m_payloadType; }
    uint16_t SequenceNumber() const { return m_sequenceNumber; }
    uint32_t SampleTick() const { return m_sampleTick; }
    uint32_t SyncSourceID() const { return m_syncSourceID; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
