#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/MeterpreterPacket.hpp"
#include "../PacketHandler.hpp"
#include "../../NetworkMiner/FileStreamAssembler.hpp"
#include <QMap>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class MeterpreterPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit MeterpreterPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<PacketParser::FiveTuple, std::shared_ptr<NetworkMiner::FileStreamAssembler>> fileStreamAssemblers;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
