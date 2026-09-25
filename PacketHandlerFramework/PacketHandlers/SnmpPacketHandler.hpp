#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/Packets/SnmpPacket.hpp"
#include "../PacketParser/Packets/UdpPacket.hpp"
#include "../PacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class SnmpPacketHandler : public AbstractPacketHandler {
public:
    explicit SnmpPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
