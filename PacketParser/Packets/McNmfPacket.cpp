#include "McNmfPacket.hpp"
#include <QStringList>
#include <numeric>
#include <stdexcept>
#include <QStringBuilder>

namespace PacketParser {
namespace Packets {

McNmfPacket::McNmfPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "MC-NMF"),
      m_parsedBytesCount(0)
{
    int index = packetStartIndex;
    const uint8_t* data = parentFrame->Data();
    size_t dataLength = parentFrame->DataLength();

    while (index <= packetEndIndex && static_cast<size_t>(index) < dataLength) {
        uint8_t recordTypeRaw = data[index++];
        if (recordTypeRaw > 0x0c) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, index - 1, index - 1, "Invalid MC-NMF record type 0x" + QString::number(recordTypeRaw, 16).rightJustified(2, '0')));
            break;
        }

        RecordType rType = static_cast<RecordType>(recordTypeRaw);

        if (rType == RecordType::Version) {
            if (static_cast<size_t>(index + 1) >= dataLength) break;
            uint8_t majorVersion = data[index++];
            uint8_t minorVersion = data[index++];
            m_records.emplace_back(RecordType::Version, QString::number(majorVersion) + "." + QString::number(minorVersion), 3);
        }
        else if (rType == RecordType::Mode) {
            if (static_cast<size_t>(index) >= dataLength) break;
            uint8_t modeRaw = data[index++];
            if (modeRaw >= 1 && modeRaw <= 4) {
                Mode mode = static_cast<Mode>(modeRaw);
                QString modeStr;
                switch (mode) {
                    case Mode::SingletonUnsized: modeStr = "SingletonUnsized"; break;
                    case Mode::Duplex: modeStr = "Duplex"; break;
                    case Mode::Simplex: modeStr = "Simplex"; break;
                    case Mode::SingletonSized: modeStr = "SingletonSized"; break;
                }
                m_records.emplace_back(RecordType::Mode, modeStr, 2);
            }
        }
        else if (rType == RecordType::Via) {
            Record* r = nullptr;
            if (TryGetVariableUtf8StringRecord(data, dataLength, index, RecordType::Via, r)) {
                m_records.push_back(*r);
                delete r;
            }
            else {
                break;
            }
        }
        else if (rType == RecordType::KnownEncoding) {
            if (static_cast<size_t>(index) >= dataLength) break;
            uint8_t encodingRaw = data[index++];
            if (encodingRaw <= 8) {
                Encoding encoding = static_cast<Encoding>(encodingRaw);
                QString encStr;
                switch (encoding) {
                    case Encoding::SOAP_1_1_UTF8: encStr = "SOAP_1_1_UTF8"; break;
                    case Encoding::SOAP_1_1_UTF16: encStr = "SOAP_1_1_UTF16"; break;
                    case Encoding::SOAP_1_1_UnicodeLittleEndian: encStr = "SOAP_1_1_UnicodeLittleEndian"; break;
                    case Encoding::SOAP_1_2_UTF8: encStr = "SOAP_1_2_UTF8"; break;
                    case Encoding::SOAP_1_2_UTF16: encStr = "SOAP_1_2_UTF16"; break;
                    case Encoding::SOAP_1_2_UnicodeLittleEndian: encStr = "SOAP_1_2_UnicodeLittleEndian"; break;
                    case Encoding::SOAP_1_2_MTOM: encStr = "SOAP_1_2_MTOM"; break;
                    case Encoding::SOAP_1_2_MC_NBFS: encStr = "SOAP_1_2_MC_NBFS"; break;
                    case Encoding::SOAP_1_2_MC_NBFSE: encStr = "SOAP_1_2_MC_NBFSE"; break;
                }
                m_records.emplace_back(RecordType::KnownEncoding, encStr, 2);
            }
        }
        else if (rType == RecordType::SizedEnvelope) {
            int recordStartIndex = index - 1;
            uint32_t envelopeLength = DecodeRecordSize(data, dataLength, index);
            if (dataLength < index + envelopeLength) {
                if (envelopeLength > MAX_NMF_PACKET_SIZE) {
                    m_trailingData = std::make_pair(index, envelopeLength);
                }
                break;
            }
            else {
                Record dataRecord(RecordType::SizedEnvelope, QString(), static_cast<int>(index + envelopeLength - recordStartIndex));
                dataRecord.data.assign(data + index, data + index + envelopeLength);
                index += static_cast<int>(envelopeLength);
                m_records.push_back(std::move(dataRecord));
            }
        }
        else if (rType == RecordType::End) {
            m_records.emplace_back(RecordType::End, QString(), 1);
        }
        else if (rType == RecordType::Fault) {
            Record* r = nullptr;
            if (TryGetVariableUtf8StringRecord(data, dataLength, index, RecordType::Fault, r)) {
                m_records.push_back(*r);
                delete r;
            }
            else {
                break;
            }
        }
        else if (rType == RecordType::UpgradeRequest) {
            Record* r = nullptr;
            if (TryGetVariableUtf8StringRecord(data, dataLength, index, RecordType::UpgradeRequest, r)) {
                m_records.push_back(*r);
                delete r;
            }
            else {
                break;
            }
        }
        else if (rType == RecordType::UpgradeResponse) {
            m_records.emplace_back(RecordType::UpgradeResponse, QString(), 1);
        }
        else if (rType == RecordType::PreambleAck) {
            m_records.emplace_back(RecordType::PreambleAck, QString(), 1);
        }
        else if (rType == RecordType::PreambleEnd) {
            m_records.emplace_back(RecordType::PreambleEnd, QString(), 1);
        }
    }

    if (!m_trailingData.has_value()) {
        int sum = 0;
        for (const auto& r : m_records) {
            sum += r.size;
        }
        m_parsedBytesCount = sum;
    }
    else {
        m_parsedBytesCount = m_trailingData.value().first - packetStartIndex;
    }
    setPacketEndIndex(packetStartIndex + m_parsedBytesCount - 1);
}

bool McNmfPacket::PacketHeaderIsComplete() const {
    return (m_records.size() > 0 || m_trailingData.has_value());
}

int McNmfPacket::ParsedBytesCount() const {
    return m_parsedBytesCount;
}

bool McNmfPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, McNmfPacket** result) {
    if (packetStartIndex >= static_cast<int>(parentFrame->DataLength()) || parentFrame->Data()[packetStartIndex] > 0x0c) {
        if (result) *result = nullptr;
        return false;
    }
    else {
        try {
            McNmfPacket* packet = new McNmfPacket(parentFrame, packetStartIndex, packetEndIndex);
            if (result) *result = packet;
            return true;
        }
        catch (...) {
            if (result) *result = nullptr;
            return false;
        }
    }
}

bool McNmfPacket::TryGetVariableUtf8StringRecord(const uint8_t* data, size_t dataLength, int& index, RecordType recordType, Record*& record) {
    int recordStartIndex = index - 1;
    uint32_t stringLength = DecodeRecordSize(data, dataLength, index);
    if (stringLength < 1) {
        return false;
    }
    if (dataLength < index + stringLength) {
        record = nullptr;
        return false;
    }
    else {
        QString s = QString::fromUtf8(reinterpret_cast<const char*>(data + index), static_cast<int>(stringLength));
        index += static_cast<int>(stringLength);
        record = new Record(recordType, s, index - recordStartIndex);
        return true;
    }
}

uint32_t McNmfPacket::DecodeRecordSize(const uint8_t* data, size_t dataLength, int& index) {
    uint32_t recordSize = 0;
    int offset = 0;
    uint8_t b = 0;
    do {
        if (static_cast<size_t>(index + offset) >= dataLength) break;
        b = data[index + offset];
        recordSize |= static_cast<uint32_t>(b & 0x7f) << (offset * 7);
        offset++;
    } while (offset < 5 && b > 0x7f);
    index += offset;
    return recordSize;
}

std::vector<AbstractPacket*> McNmfPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
