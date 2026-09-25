#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/NetBiosDatagramServicePacket.hpp"
#include "../PacketParser/Packets/UdpPacket.hpp"
#include "../PacketParser/Packets/IPv4Packet.hpp"
#include "../PacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class NetBiosDatagramServicePacketHandler : public AbstractPacketHandler {
public:
    explicit NetBiosDatagramServicePacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
