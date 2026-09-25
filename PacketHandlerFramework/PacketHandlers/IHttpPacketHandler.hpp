#pragma once
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QHash>

namespace PacketHandlerFramework {
namespace PacketHandlers {
class IHttpPacketHandler {
public:
    virtual ~IHttpPacketHandler() = default;

    virtual bool ExtractHttpData(HttpPacket httpPacket, TcpPacket tcpPacket, FiveTuple fiveTuple, bool transferIsClientToServer, PacketHandler mainPacketHandler) = 0;
    virtual bool ExtractHttpData(Http2Packet http2Packet, string> headers, TcpPacket tcpPacket, FiveTuple fiveTuple, bool transferIsClientToServer, PacketHandler mainPacketHandler) = 0;
    virtual void Reset() = 0;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
