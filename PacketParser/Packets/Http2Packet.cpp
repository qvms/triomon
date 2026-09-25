#include "Http2Packet.hpp"
#include "../Utils.hpp"
#include <cstring>

namespace PacketParser {
namespace Packets {

const QString Http2Packet::CLIENT_CONNECTION_PREFACE_STRING = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
const std::vector<uint8_t> Http2Packet::CLIENT_CONNECTION_PREFACE_BYTES = {
    0x50, 0x52, 0x49, 0x20, 0x2a, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2f, 0x32, 0x2e, 0x30, 0x0d, 0x0a, 0x0d, 0x0a, 0x53, 0x4d, 0x0d, 0x0a, 0x0d, 0x0a
};

Http2Packet::Http2Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "HTTP/2")
    , m_prefaceBytes(0)
{
    int dataLength = packetEndIndex - packetStartIndex + 1;
    if (dataLength >= static_cast<int>(CLIENT_CONNECTION_PREFACE_BYTES.size()) &&
        std::memcmp(parentFrame->Data() + packetStartIndex, CLIENT_CONNECTION_PREFACE_BYTES.data(), CLIENT_CONNECTION_PREFACE_BYTES.size()) == 0) {
        m_prefaceBytes = CLIENT_CONNECTION_PREFACE_BYTES.size();
    }

    uint8_t lenBuf[4] = {0, parentFrame->Data()[packetStartIndex + m_prefaceBytes],
                            parentFrame->Data()[packetStartIndex + m_prefaceBytes + 1],
                            parentFrame->Data()[packetStartIndex + m_prefaceBytes + 2]};
    m_length = static_cast<int>(Utils::ByteConverter::ToUInt32(lenBuf, 0));
    
    m_type = static_cast<FrameType>(parentFrame->Data()[packetStartIndex + m_prefaceBytes + 3]);
    m_flagsRaw = parentFrame->Data()[packetStartIndex + m_prefaceBytes + 4];
    m_streamIdentifier = static_cast<int>(Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + m_prefaceBytes + 5)) & 0x7fffffff;
}

std::vector<uint8_t> Http2Packet::Payload() const {
    std::vector<uint8_t> payloadData;
    if (m_length > 0 && PacketStartIndex() + m_prefaceBytes + 9 + m_length <= static_cast<int>(ParentFrame()->DataLength())) {
        payloadData.resize(m_length);
        std::memcpy(payloadData.data(), ParentFrame()->Data() + PacketStartIndex() + m_prefaceBytes + 9, m_length);
    }
    return payloadData;
}

bool Http2Packet::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;

    int index = packetStartIndex;
    int dataLength = packetEndIndex - index + 1;

    if (dataLength >= static_cast<int>(CLIENT_CONNECTION_PREFACE_BYTES.size()) &&
        std::memcmp(parentFrame->Data() + index, CLIENT_CONNECTION_PREFACE_BYTES.data(), CLIENT_CONNECTION_PREFACE_BYTES.size()) == 0) {
        index += CLIENT_CONNECTION_PREFACE_BYTES.size();
    }

    if (static_cast<int>(parentFrame->DataLength()) < index + 9) {
        return false;
    }

    uint8_t lenBuf[4] = {0, parentFrame->Data()[index], parentFrame->Data()[index + 1], parentFrame->Data()[index + 2]};
    int length = static_cast<int>(Utils::ByteConverter::ToUInt32(lenBuf, 0));

    if (static_cast<int>(parentFrame->DataLength()) < index + 9 + length) {
        return false;
    }

    uint8_t typeRaw = parentFrame->Data()[index + 3];
    if (typeRaw > 0x9) { // Out of bounds for enum FrameType
        return false;
    }

    try {
        *result = new Http2Packet(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

int Http2Packet::ParsedBytesCount() const {
    if (static_cast<int>(ParentFrame()->DataLength()) >= PacketStartIndex() + m_prefaceBytes + 9 + m_length) {
        return m_prefaceBytes + 9 + m_length;
    }
    return 0;
}

std::vector<AbstractPacket*> Http2Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
