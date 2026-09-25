#pragma once

#include <cstdint>
#include <QByteArray>
#include <QMap>
#include <memory>

class NetworkTcpSession;
namespace PacketParser {

class TcpDataStream {
public:
    class VirtualTcpData {
    private:
        TcpDataStream* tcpDataStream;
        uint16_t sourcePort;
        uint16_t destinationPort;
        int nPackets;
    public:
        VirtualTcpData(TcpDataStream* stream, uint16_t srcPort, uint16_t dstPort);
        int PacketCount() const { return nPackets; }
        int ByteCount() const;
        uint32_t FirstPacketSequenceNumber() const;
        bool TryAppendNextPacket();
        QByteArray GetTcpHeader() const;
        QByteArray GetBytes(bool includeTcpHeader) const;
    };

private:
    uint32_t initialTcpSequenceNumber;
    uint32_t expectedTcpSequenceNumber;
    uint16_t sourcePort, destinationPort;

    QMap<uint32_t, QByteArray> dataList;
    int dataListMaxSize;
    int totalByteCount;
    std::shared_ptr<VirtualTcpData> virtualTcpData;

public:
    TcpDataStream(uint32_t initialSeqNum, uint16_t srcPort, uint16_t dstPort);
    ~TcpDataStream();

    int TotalByteCount() const { return totalByteCount; }
    int DataSegmentBufferCount() const { return dataList.size(); }
    int DataSegmentBufferMaxSize() const { return dataListMaxSize; }

    void SetInitialTcpSequenceNumber(uint32_t seq) { initialTcpSequenceNumber = seq; }
    uint32_t ExpectedTcpSequenceNumber() const { return expectedTcpSequenceNumber; }

    bool HasMissingSegments() const;
    void AddTcpData(uint32_t seq, const QByteArray& data);
    void RemoveData(int bytesToRemove);
    void Clear();

    int CountBytesToRead() const;
    int CountPacketsToRead() const;
    VirtualTcpData GetAllAvailableTcpData();
    std::shared_ptr<VirtualTcpData> GetNextVirtualTcpData();
};

} // namespace PacketParser
