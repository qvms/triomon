#pragma once
#include "AbstractPacket.hpp"
#include <vector>
#include <QString>
#include <QHostAddress>

namespace PacketParser {
namespace Packets {

class DnsPacket : public AbstractPacket {
public:
    enum class RRTypes : uint16_t {
        HostAddress = 0x0001,
        CNAME = 0x0005,
        DomainNamePointer = 0x000c,
        NB = 0x0020,
        NBSTAT = 0x0021
    };

    class HeaderFlags {
    public:
        enum class OperationCodes : uint8_t { Query = 0, InverseQuery = 1, ServerStatusRequest = 2 };
        enum class ResultCodes : uint8_t { NoErrorCondition = 0, FormatError = 1, ServerFailure = 2, NameError = 3, NotImplemented = 4, Refused = 5 };

    private:
        uint16_t headerData;

    public:
        HeaderFlags(uint16_t value) : headerData(value) {}
        HeaderFlags() : headerData(0) {}

        bool Response() const { return ((headerData >> 15) == 1); }
        uint8_t OperationCode() const { return (uint8_t)((headerData >> 11) & 0x000F); }
        bool Truncated() const { return ((headerData >> 9) & 0x0001) == 1; }
        bool RecursionDesired() const { return ((headerData >> 8) & 0x0001) == 1; }
        uint8_t ResultCode() const { return (uint8_t)(headerData & 0x000F); }
    };

    class NameLabel {
    private:
        int labelStartPosition;
        uint8_t labelByteCount;
        QString decodedName;

    public:
        NameLabel(const uint8_t* sourceData, int labelStartPosition);
        uint8_t LabelByteCount() const { return labelByteCount; }
        QString ToString() const { return decodedName; }
    };

    class ResourceRecord {
    private:
        QStringList answerRequestedNameDecoded;
        uint16_t answerType;
        uint16_t answerClass;
        uint32_t answerTimeToLive;
        uint16_t answerDataLength;
        QStringList answerRepliedNameDecoded;
        DnsPacket* parentPacket;
        int recordByteCount;

    public:
        ResourceRecord(DnsPacket* parentPacket, int startIndex);

        DnsPacket* ParentPacket() const { return parentPacket; }
        uint16_t Type() const { return answerType; }
        uint32_t TimeToLive() const { return answerTimeToLive; }
        int ByteCount() const { return recordByteCount; }
        QHostAddress IP() const;
        QString PrimaryName() const;
        QString DNS() const;
    };

private:
    uint16_t transactionID;
    HeaderFlags headerFlags;
    uint16_t questionCount;
    uint16_t answerCount;
    uint16_t nameServerCount;
    uint16_t additionalCount;

    int questionSectionByteCount;
    QStringList questionNameDecoded;
    uint16_t questionType;
    uint16_t questionClass;

    std::vector<ResourceRecord*> answerRecords;

public:
    DnsPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    ~DnsPacket();

    uint16_t TransactionId() const { return transactionID; }
    HeaderFlags Flags() const { return headerFlags; }
    const std::vector<ResourceRecord*>& AnswerRecords() const { return answerRecords; }
    QString QueriedDnsName() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    static std::vector<NameLabel> GetNameLabelList(const uint8_t* sourceData, int baseOffset, int relativeStartIndex);
};

} // namespace Packets
} // namespace PacketParser
