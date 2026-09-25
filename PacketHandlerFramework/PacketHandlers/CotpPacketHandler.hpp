#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/CotpPacket.hpp"
#include "../../PacketParser/NetworkTcpSession.hpp"
#include <QHash>
#include <QByteArray>
#include <memory>
#include <QString>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class CotpPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    CotpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~CotpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList);
    void Reset();

private:
    QString GetTsapString(const QByteArray& data, int index, int length, bool& tsapIsAsciiString);
    bool TryGetRdpCookie(const QByteArray& data, int offset, QString& rdpCookie);

    QHash<std::shared_ptr<PacketParser::NetworkTcpSession>, QByteArray> cotpDataSegments;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
