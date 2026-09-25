#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/LpdPacket.hpp"
#include "../PacketHandler.hpp"
#include "../../NetworkMiner/FileStreamAssembler.hpp"
#include <QMap>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class LpdPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit LpdPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<PacketParser::NetworkTcpSession*, std::shared_ptr<NetworkMiner::FileStreamAssembler>> assemblerList;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
