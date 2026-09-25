#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/SocksPacket.hpp"
#include "../PacketParser/Packets/TcpPacket.hpp"
#include "../PacketHandler.hpp"
#include <QMap>
#include <QPair>
#include <QHostAddress>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class SocksPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit SocksPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<PacketParser::NetworkTcpSession*, QPair<QHostAddress, quint16>> socksConnectIpPorts;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
