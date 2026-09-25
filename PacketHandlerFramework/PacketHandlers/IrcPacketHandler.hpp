#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/IrcPacket.hpp"
#include "../PacketParser/Packets/TcpPacket.hpp"
#include "../NetworkHost.hpp"
#include <QMap>
#include <QString>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class IrcPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit IrcPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    class IrcSession {
    public:
        QString User;
        QString Nick;
        QString Pass;

        std::shared_ptr<NetworkCredential> GetCredential(NetworkHost* sourceHost, NetworkHost* destinationHost, const QDateTime& timestamp) const;
    };

    QMap<PacketParser::NetworkTcpSession*, std::shared_ptr<IrcSession>> ircSessionList;
    static const char CTCP_DELIMITER = '\x01';
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
