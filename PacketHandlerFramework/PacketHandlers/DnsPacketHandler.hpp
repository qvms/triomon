#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/IPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/DnsPacket.hpp"
#include "../../PacketParser/NetworkHost.hpp"
#include "../../PacketParser/Packets/IIPPacket.hpp"
#include "../../PacketParser/Packets/ITransportLayerPacket.hpp"
#include <QSet>
#include <QString>
#include <memory>
#include <QHostAddress>
#include <QList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class DnsPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::IPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    DnsPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~DnsPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    
    // IPacketHandler
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

    // ITcpSessionPacketHandler
    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;

private:
    void ExtractDomainNameRecords(std::shared_ptr<PacketParser::Packets::DnsPacket> dnsPacket, const QList<std::shared_ptr<PacketParser::Packets::DnsPacket::ResourceRecord>>& rr, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, std::shared_ptr<PacketParser::Packets::IIPPacket> ipPacket, std::shared_ptr<PacketParser::Packets::ITransportLayerPacket> transportLayerPacket);

    class DNSBL {
    public:
        static bool TryParse(const QString& domain, QHostAddress& ip, QString& service);
    private:
        static const QSet<QString> DNSBL_DOMAINS;
    };
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
