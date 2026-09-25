#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/IPacketHandler.hpp"
#include "../../PacketParser/Packets/HpSwitchProtocolPacket.hpp"
#include "../../PacketParser/NetworkHost.hpp"
#include <QList>
#include <memory>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class HpSwitchProtocolPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::IPacketHandler {
public:
    HpSwitchProtocolPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~HpSwitchProtocolPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    
    // IPacketHandler
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    void ExtractDataInternal(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::Packets::HpSwitchProtocolPacket::HpSwField> hpswField);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
