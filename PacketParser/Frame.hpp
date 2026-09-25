#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <memory>
#include "PcapFrameDataLinkTypeEnum.hpp"

namespace PacketParser {

namespace Packets {
class AbstractPacket;
}

class Frame {
public:
    static constexpr int MAX_FRAME_SIZE = 66000;

    struct Error {
        const Frame* frame;
        int errorStartIndex;
        int errorEndIndex;
        QString description;

        Error(const Frame* f, int start, int end, const QString& desc)
            : frame(f), errorStartIndex(start), errorEndIndex(end), description(desc) {}
    };

private:
    int64_t m_frameNumber;
    QDateTime m_timestamp;
    const uint8_t* m_data;
    size_t m_dataLength;
    bool m_quickParse;
    QVariant m_tag;
    PacketParser::DataLinkTypeEnum m_dataLinkType;
    
    std::map<int, Packets::AbstractPacket*> m_packetList; // we'll manage memory or use unique_ptr later
    bool m_precomputePacketList;
    std::vector<Error> m_errorList;

public:
    Frame(QDateTime timestamp, const uint8_t* data, size_t dataLength, int64_t frameNumber, PacketParser::DataLinkTypeEnum dataLinkType, bool precomputePacketList = true, bool quickParse = false, int maxFrameSize = MAX_FRAME_SIZE);
    ~Frame();
    
    int64_t FrameNumber() const { return m_frameNumber; }
    QDateTime Timestamp() const { return m_timestamp.toLocalTime(); }
    const uint8_t* Data() const { return m_data; }
    size_t DataLength() const { return m_dataLength; }
    bool QuickParse() const { return m_quickParse; }
    PacketParser::DataLinkTypeEnum DataLinkType() const { return m_dataLinkType; }
    QVariant Tag() const { return m_tag; }
    void setTag(const QVariant& tag) { m_tag = tag; }

    const std::vector<Error>& Errors() const { return m_errorList; }
    void addError(const Error& err) { m_errorList.push_back(err); }

    const std::map<int, Packets::AbstractPacket*>& GetPackets() const { return m_packetList; }

};

} // namespace PacketParser
