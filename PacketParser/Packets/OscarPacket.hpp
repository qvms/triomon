#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class OscarPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    class TagLengthValue {
    public:
        enum class IcbmTag : uint16_t {
            IM_DATA = 2, REQUEST_HOST_ACK = 3, AUTO_RESPONSE = 4, DATA = 5, STORE = 6, WANT_EVENTS = 11, BART = 13, HOST_IM_ID = 16, HOST_IM_ARGS = 17, SEND_TIME = 22, FRIENDLY_NAME = 23, ANONYMOUS = 24, WIDGET_NAME = 25
        };
        enum class IcbmImDataTag : uint16_t {
            IM_CAPABILITIES = 0x0501, IM_TEXT = 0x0101, MIME_ARRAY = 0x0D01
        };

        uint16_t Tag;
        uint16_t Length;
        std::vector<uint8_t> Value;

        TagLengthValue(const uint8_t* data, size_t dataLen, int& offset);
        TagLengthValue(const std::vector<uint8_t>& data, int& offset);
    };

private:
    uint8_t m_flapChannel;
    uint16_t m_seqNumber;
    uint16_t m_bytesToParse;

    // SNAC
    uint16_t m_snacFamily;
    uint16_t m_snacSubType;

    QString m_sourceLoginId;
    QString m_destinationLoginId;
    QString m_imText;
    bool m_isValid;

    OscarPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    uint8_t FlapChannel() const { return m_flapChannel; }
    uint16_t BytesToParse() const { return m_bytesToParse; }
    QString DestinationLoginId() const { return m_destinationLoginId; }
    QString SourceLoginId() const { return m_sourceLoginId; }
    QString ImText() const { return m_imText; }
    bool IsValid() const { return m_isValid; }

    bool PacketHeaderIsComplete() const override { return true; }
    int ParsedBytesCount() const override { return 6 + m_bytesToParse; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
