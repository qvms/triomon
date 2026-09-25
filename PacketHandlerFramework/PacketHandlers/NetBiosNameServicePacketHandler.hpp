#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/Packets/NetBiosNameServicePacket.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class NetBiosNameServicePacketHandler : public AbstractPacketHandler {
public:
    explicit NetBiosNameServicePacketHandler(PacketHandler* mainPacketHandler);
    ~NetBiosNameServicePacketHandler() override = default;

    QList<std::type_index> getParsedTypes() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<PacketParser::Packets::AbstractPacket*>& packetList) override;
    void Reset() override;

private:
    void ExtractData(PacketParser::Packets::NetBiosNameServicePacket* netBiosNameServicePacket, NetworkHost* sourceHost, NetworkHost* destinationHost, PacketParser::Packets::ITransportLayerPacket* transportLayerPacket);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
