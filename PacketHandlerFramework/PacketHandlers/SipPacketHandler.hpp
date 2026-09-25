#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/SipPacket.hpp"
#include "../PacketParser/Packets/UdpPacket.hpp"
#include "../PacketParser/Packets/TcpPacket.hpp"
#include "../PacketHandler.hpp"
#include <QMap>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class SipPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit SipPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
