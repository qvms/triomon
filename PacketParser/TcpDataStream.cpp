#include "TcpDataStream.hpp"

namespace PacketParser {

TcpDataStream::VirtualTcpData::VirtualTcpData(TcpDataStream* stream, uint16_t srcPort, uint16_t dstPort)
    : tcpDataStream(stream), sourcePort(srcPort), destinationPort(dstPort), nPackets(1) {
}

int TcpDataStream::VirtualTcpData::ByteCount() const {
    if (nPackets <= 0 || tcpDataStream->dataList.isEmpty()) return 0;
    auto it = tcpDataStream->dataList.begin();
    for (int i = 0; i < nPackets - 1; ++i) { ++it; }
    return (it.key() + it.value().length()) - tcpDataStream->dataList.firstKey();
}

uint32_t TcpDataStream::VirtualTcpData::FirstPacketSequenceNumber() const {
    return tcpDataStream->dataList.firstKey();
}

bool TcpDataStream::VirtualTcpData::TryAppendNextPacket() {
    int maxPacketFragments = 6;
    if (tcpDataStream->CountBytesToRead() > ByteCount() && tcpDataStream->CountPacketsToRead() > nPackets && nPackets < maxPacketFragments) {
        nPackets++;
        return true;
    }
    return false;
}

QByteArray TcpDataStream::VirtualTcpData::GetTcpHeader() const {
    QByteArray header(20, 0);
    // Simple stub for header parsing if ever needed.
    return header;
}

QByteArray TcpDataStream::VirtualTcpData::GetBytes(bool includeTcpHeader) const {
    QByteArray result;
    if (includeTcpHeader) {
        result.append(GetTcpHeader());
    }
    auto it = tcpDataStream->dataList.begin();
    for (int i = 0; i < nPackets && it != tcpDataStream->dataList.end(); ++i, ++it) {
        result.append(it.value());
    }
    return result;
}

TcpDataStream::TcpDataStream(uint32_t initialSeqNum, uint16_t srcPort, uint16_t dstPort)
    : initialTcpSequenceNumber(initialSeqNum), expectedTcpSequenceNumber(initialSeqNum),
      sourcePort(srcPort), destinationPort(dstPort), dataListMaxSize(64), totalByteCount(0), virtualTcpData(nullptr) {
}

TcpDataStream::~TcpDataStream() {
}

bool TcpDataStream::HasMissingSegments() const {
    return totalByteCount < static_cast<int>(expectedTcpSequenceNumber - initialTcpSequenceNumber);
}

void TcpDataStream::AddTcpData(uint32_t seq, const QByteArray& data) {
    if (data.isEmpty()) return;
    if (!dataList.contains(seq)) {
        dataList.insert(seq, data);
        totalByteCount += data.length();
        if (expectedTcpSequenceNumber == seq) {
            expectedTcpSequenceNumber += data.length();
            while (dataList.contains(expectedTcpSequenceNumber)) {
                expectedTcpSequenceNumber += dataList[expectedTcpSequenceNumber].length();
            }
        }
    }
}

void TcpDataStream::RemoveData(int bytesToRemove) {
    int bytesRemoved = 0;
    while (bytesRemoved < bytesToRemove && !dataList.isEmpty()) {
        auto it = dataList.begin();
        if (bytesRemoved + it.value().length() <= bytesToRemove) {
            bytesRemoved += it.value().length();
            dataList.erase(it);
        } else {
            int toRemoveNow = bytesToRemove - bytesRemoved;
            uint32_t newSeq = it.key() + toRemoveNow;
            QByteArray newData = it.value().mid(toRemoveNow);
            dataList.erase(it);
            dataList.insert(newSeq, newData);
            bytesRemoved += toRemoveNow;
        }
    }
    if (virtualTcpData) {
        virtualTcpData.reset();
    }
}

void TcpDataStream::Clear() {
    dataList.clear();
    totalByteCount = 0;
    virtualTcpData.reset();
}

int TcpDataStream::CountBytesToRead() const {
    if (dataList.isEmpty()) return 0;
    uint32_t firstSeq = dataList.firstKey();
    if (firstSeq != initialTcpSequenceNumber && firstSeq != expectedTcpSequenceNumber && dataList.size() == 1) { // Approximate fallback
        return dataList.first().length();
    }
    if (expectedTcpSequenceNumber > firstSeq)
        return expectedTcpSequenceNumber - firstSeq;
    return 0; // Missing segments before anything can be read
}

int TcpDataStream::CountPacketsToRead() const {
    int count = 0;
    uint32_t expected = dataList.isEmpty() ? 0 : dataList.firstKey();
    for (auto it = dataList.begin(); it != dataList.end(); ++it) {
        if (it.key() == expected) {
            count++;
            expected += it.value().length();
        } else {
            break;
        }
    }
    return count;
}

TcpDataStream::VirtualTcpData TcpDataStream::GetAllAvailableTcpData() {
    VirtualTcpData vtd(this, sourcePort, destinationPort);
    while (vtd.TryAppendNextPacket()) {}
    return vtd;
}

std::shared_ptr<TcpDataStream::VirtualTcpData> TcpDataStream::GetNextVirtualTcpData() {
    if (!virtualTcpData) {
        if (CountBytesToRead() > 0) {
            virtualTcpData = std::make_shared<VirtualTcpData>(this, sourcePort, destinationPort);
        }
    } else {
        if (!virtualTcpData->TryAppendNextPacket()) {
            return nullptr;
        }
    }
    return virtualTcpData;
}

} // namespace PacketParser
