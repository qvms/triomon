#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/SshPacket.hpp"
#include "../PacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class SshPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit SshPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
