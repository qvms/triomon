#pragma once

#include "AbstractTlsHandshakePacketHandler.hpp"
#include "../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../PacketParser/Packets/TlsRecordPacket.hpp"
#include "../PacketParser/Packets/TcpPacket.hpp"
#include "../PacketHandler.hpp"
#include <QMap>
#include <QPair>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class TlsRecordPacketHandler : public AbstractTlsHandshakePacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    explicit TlsRecordPacketHandler(PacketHandler* mainPacketHandler, const QMap<QString, std::shared_ptr<Fingerprints::IJa4Fingerprint>>& ja4Fingerprints, bool verifyX509Certificates = false);

    QList<int> ParsedTypes() const override;
    ApplicationLayerProtocol HandledProtocol() const override;

    int ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<PacketParser::FiveTuple, QPair<QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>, QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>>> tlsRecordFragmentCache;

    void RemoveParsedTlsRecordsFromList(QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>& recordList, int parsedBytes, std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
