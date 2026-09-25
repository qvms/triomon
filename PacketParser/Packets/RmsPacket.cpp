#include "RmsPacket.hpp"
#include <cstring>
#include <QDebug>

namespace PacketParser {
namespace Packets {

const uint8_t UTF8_BYTE_ORDER_MARK[] = { 0xef, 0xbb, 0xbf };
const uint8_t XML_HEADER[] = { 0x3c, 0x3f, 0x78, 0x6d };

bool startsWith(const uint8_t* data, size_t dataLen, int startIndex, const uint8_t* prefix, size_t prefixLen) {
    if (startIndex < 0 || startIndex + prefixLen > dataLen) return false;
    return std::memcmp(data + startIndex, prefix, prefixLen) == 0;
}

RmsPacket::RmsPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "RMS")
    , NextSegmentLength(0)
    , HasNextSegmentLength(false)
    , m_packetHeaderIsComplete(false)
    , m_parsedBytesCount(0)
{
    if (PacketLength() == 8) {
        NextSegmentLength = Utils::ByteConverter::ToUInt64(parentFrame->Data(), packetStartIndex, true);
        if (NextSegmentLength > 0 && NextSegmentLength < 1024 * 1024) {
            HasNextSegmentLength = true;
            m_packetHeaderIsComplete = true;
            m_parsedBytesCount = 8;
        }
    } else if (startsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, UTF8_BYTE_ORDER_MARK, sizeof(UTF8_BYTE_ORDER_MARK))) {
        QString xmlPayload = QString::fromUtf8(reinterpret_cast<const char*>(parentFrame->Data() + packetStartIndex + 3), PacketLength() - 3);
        Payload.setContent(xmlPayload);
        m_packetHeaderIsComplete = true;
        m_parsedBytesCount = PacketLength();
    } else if (startsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, XML_HEADER, sizeof(XML_HEADER))) {
        QString xmlPayload = QString::fromUtf8(reinterpret_cast<const char*>(parentFrame->Data() + packetStartIndex), PacketLength());
        Payload.setContent(xmlPayload);
        m_packetHeaderIsComplete = true;
        m_parsedBytesCount = PacketLength();
    } else {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Not RMS"));
        }
    }
}

bool RmsPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    if (packetEndIndex - packetStartIndex == 7) { // length 8
        try {
            uint64_t nextSegmentLength = Utils::ByteConverter::ToUInt64(parentFrame->Data(), packetStartIndex, true);
            if (nextSegmentLength > 0 && nextSegmentLength < 1024 * 1024) {
                *result = new RmsPacket(parentFrame, packetStartIndex, packetEndIndex);
                return true;
            }
        } catch (...) {}
    } else if (startsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, UTF8_BYTE_ORDER_MARK, sizeof(UTF8_BYTE_ORDER_MARK)) ||
               startsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, XML_HEADER, sizeof(XML_HEADER))) {
        try {
            *result = new RmsPacket(parentFrame, packetStartIndex, packetEndIndex);
            return true;
        } catch (...) {}
    }
    
    return false;
}

std::vector<AbstractPacket*> RmsPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
