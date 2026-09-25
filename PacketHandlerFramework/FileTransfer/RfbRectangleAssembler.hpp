#pragma once

#include <QList>
#include <QDateTime>
#include <functional>
#include "PacketHandlerFramework/FileTransfer/ITcpStreamAssembler.hpp"
#include "PacketParser/TcpDataStream.hpp"
#include "PacketParser/NetworkTcpSession.hpp"
#include "PacketParser/Packets/RfbPacket.hpp"

namespace PacketHandlerFramework {
namespace FileTransfer {

class RfbRectangleAssembler : public ITcpStreamAssembler {
private:
    uint32_t startSequenceNumber;
    PacketParser::TcpDataStream tcpDataStream;
    PacketParser::Packets::RfbPacket::VncPixelFormat* pixelFormat;
    PacketParser::NetworkTcpSession* session;
    QDateTime startTime;
    long startFrameNumber;

    int totalRectangles;
    QList<PacketParser::Packets::RfbPacket::Rectangle*> parsedRectangles;

public:
    RfbRectangleAssembler(PacketParser::Packets::RfbPacket::FrameBufferUpdatePacket* fbUpdatePacket, 
                          uint32_t startSequenceNumber, bool clientToServer, 
                          PacketParser::NetworkTcpSession* session, 
                          PacketParser::Packets::RfbPacket::VncPixelFormat* pixelFormat);
    ~RfbRectangleAssembler() override;

    int TotalRectangles() const { return totalRectangles; }
    void SetTotalRectangles(int count) { totalRectangles = count; }
    const QList<PacketParser::Packets::RfbPacket::Rectangle*>& ParsedRectangles() const { return parsedRectangles; }

    bool IsCompleted() const;
    std::function<void(PacketParser::NetworkTcpSession*, long, QDateTime, const QList<PacketParser::Packets::RfbPacket::Rectangle*>&)> OnFinish;

    void addData(const QByteArray& data, quint32 sequenceNumber) override;
    void finishAssembling() override;
    void Clear();
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
