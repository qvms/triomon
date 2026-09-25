#pragma once

#include "AbstractTlsHandshakePacketHandler.hpp"
#include "../PacketParser/Packets/QuicPacket.hpp"
#include <QMap>
#include <QString>
#include <QList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class QuicPacketHandler : public AbstractTlsHandshakePacketHandler {
public:
    explicit QuicPacketHandler(PacketHandler* mainPacketHandler, const QMap<QString, std::shared_ptr<Fingerprints::IJa4Fingerprint>>& ja4Fingerprints);

    QList<int> ParsedTypes() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<QString, QList<std::shared_ptr<PacketParser::Packets::QuicPacket::InitialPacket>>> streamInitials;

    void ExtractHandshakeData(std::shared_ptr<PacketParser::Packets::TlsRecordPacket::HandshakePacket> handshake, NetworkHost* sourceHost, NetworkHost* destinationHost, std::shared_ptr<PacketParser::ITransportLayerPacket> transportLayerPacket, bool transferIsClientToServer = true);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
