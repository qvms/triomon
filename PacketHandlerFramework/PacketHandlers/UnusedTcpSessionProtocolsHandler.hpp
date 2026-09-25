#pragma once
#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include <QList>
#include <memory>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class UnusedTcpSessionProtocolsHandler : public AbstractPacketHandler, public ITcpSessionPacketHandler {
public:
    explicit UnusedTcpSessionProtocolsHandler(PacketHandler* mainPacketHandler);
    virtual ~UnusedTcpSessionProtocolsHandler() = default;

    QList<int> ParsedTypes() const override;
    PacketParser::ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
