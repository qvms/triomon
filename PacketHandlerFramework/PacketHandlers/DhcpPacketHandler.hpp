#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/IPacketHandler.hpp"
#include "../../PacketParser/Packets/DhcpPacket.hpp"
#include "../../PacketParser/NetworkHost.hpp"
#include <QMap>
#include <QString>
#include <memory>
#include <QHostAddress>
#include <QList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class DhcpPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::IPacketHandler {
public:
    DhcpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~DhcpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    
    // Simulating IPacketHandler extract
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList);
    void Reset();

private:
    void ExtractDataInternal(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, std::shared_ptr<PacketParser::Packets::DhcpPacket> dhcpPacket);

    QMap<QString, QHostAddress> previousIpList;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
