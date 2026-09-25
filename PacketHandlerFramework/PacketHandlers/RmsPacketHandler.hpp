#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/RmsPacket.hpp"
#include "../PacketParser/Packets/TcpPacket.hpp"
#include "../PacketHandler.hpp"
#include "../../NetworkMiner/FileStreamAssembler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class RmsPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit RmsPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
