#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/McNmfPacket.hpp"
#include "../PacketHandler.hpp"
#include <QMap>
#include <QString>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class McNmfPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit McNmfPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    class McNmfSessionEncoding {
    public:
        QString Encoding;
    };

    QMap<PacketParser::NetworkTcpSession*, std::shared_ptr<McNmfSessionEncoding>> sessionEncodings;

    bool ParseFileData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, std::shared_ptr<PacketParser::Packets::McNmfPacket> mcNmfPacket, const QString& encoding);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
