#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"

namespace PacketParser {
namespace Packets {

class Http2Packet : public AbstractPacket, public virtual ISessionPacket {
public:
    static const QString CLIENT_CONNECTION_PREFACE_STRING;
    static const std::vector<uint8_t> CLIENT_CONNECTION_PREFACE_BYTES;

    enum class FrameType : uint8_t {
        DATA = 0x0,
        HEADERS = 0x1,
        PRIORITY = 0x2,
        RST_STREAM = 0x3,
        SETTINGS = 0x4,
        PUSH_PROMISE = 0x5,
        PING = 0x6,
        GOAWAY = 0x7,
        WINDOW_UPDATE = 0x8,
        CONTINUATION = 0x9
    };

private:
    int m_prefaceBytes;
    int m_length;
    FrameType m_type;
    uint8_t m_flagsRaw;
    int m_streamIdentifier;

    Http2Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    int Length() const { return m_length; }
    FrameType Type() const { return m_type; }
    uint8_t FlagsRaw() const { return m_flagsRaw; }
    bool FlagEndStream() const { return (m_flagsRaw & 0x01) == 0x01; }
    bool FlagEndHeaders() const { return (m_flagsRaw & 0x04) == 0x04; }
    bool FlagPadded() const { return (m_flagsRaw & 0x08) == 0x08; }
    bool FlagPriority() const { return (m_flagsRaw & 0x20) == 0x20; }
    int StreamIdentifier() const { return m_streamIdentifier; }

    std::vector<uint8_t> Payload() const;

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override { return ParsedBytesCount() > 0; }
    int ParsedBytesCount() const override;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
