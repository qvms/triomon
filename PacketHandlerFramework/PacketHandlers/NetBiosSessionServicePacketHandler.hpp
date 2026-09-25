#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/NetBiosSessionService.hpp"
#include "../../PacketParser/NetworkTcpSession.hpp"
#include <memory>
#include <QList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class NetBiosSessionServicePacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    NetBiosSessionServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~NetBiosSessionServicePacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
