#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QString>
#include <vector>
#include <cstdint>
#include <optional>

namespace PacketParser {
namespace Packets {

class McNmfPacket : public AbstractPacket, public ISessionPacket {
public:
    enum class RecordType : uint8_t {
        Version = 0x00,
        Mode = 0x01,
        Via = 0x02,
        KnownEncoding = 0x03,
        ExtensibleEncoding = 0x04,
        UnsizedEnvelope = 0x05,
        SizedEnvelope = 0x06,
        End = 0x07,
        Fault = 0x08,
        UpgradeRequest = 0x09,
        UpgradeResponse = 0x0A,
        PreambleAck = 0x0B,
        PreambleEnd = 0x0C,
    };

    enum class Mode : uint8_t {
        SingletonUnsized = 0x01,
        Duplex = 0x02,
        Simplex = 0x03,
        SingletonSized = 0x04
    };

    enum class Encoding : uint8_t {
        SOAP_1_1_UTF8 = 0x00,
        SOAP_1_1_UTF16 = 0x01,
        SOAP_1_1_UnicodeLittleEndian = 0x02,
        SOAP_1_2_UTF8 = 0x03,
        SOAP_1_2_UTF16 = 0x04,
        SOAP_1_2_UnicodeLittleEndian = 0x05,
        SOAP_1_2_MTOM = 0x06,
        SOAP_1_2_MC_NBFS = 0x07,
        SOAP_1_2_MC_NBFSE = 0x08,
    };

    class Record {
    public:
        RecordType recordType;
        QString recordStringValue;
        int size;
        std::vector<uint8_t> data;

        Record(RecordType type, const QString& strVal, int sz)
            : recordType(type), recordStringValue(strVal), size(sz) {}
    };

private:
    static const uint32_t MAX_NMF_PACKET_SIZE = 1500;
    std::vector<Record> m_records;
    std::optional<std::pair<int, uint32_t>> m_trailingData; // offset, length
    int m_parsedBytesCount;

    bool TryGetVariableUtf8StringRecord(const uint8_t* data, size_t dataLength, int& index, RecordType recordType, Record*& record);
    static uint32_t DecodeRecordSize(const uint8_t* data, size_t dataLength, int& index);

public:
    McNmfPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    virtual ~McNmfPacket() = default;

    bool PacketHeaderIsComplete() const override;
    int ParsedBytesCount() const override;

    const std::vector<Record>& Records() const { return m_records; }
    std::optional<std::pair<int, uint32_t>> TrailingData() const { return m_trailingData; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, McNmfPacket** result);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
