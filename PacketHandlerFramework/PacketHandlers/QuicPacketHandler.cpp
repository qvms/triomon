#include "QuicPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

QuicPacketHandler::QuicPacketHandler(PacketHandler* mainPacketHandler, const QMap<QString, std::shared_ptr<Fingerprints::IJa4Fingerprint>>& ja4Fingerprints)
    : AbstractTlsHandshakePacketHandler(mainPacketHandler, ja4Fingerprints, false) {
}

QList<int> QuicPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::QuicPacket>() };
}

void QuicPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::QuicPacket> quicPacket;
    std::shared_ptr<PacketParser::ITransportLayerPacket> transportLayerPacket;
    
    if (TryGetPackets<PacketParser::Packets::QuicPacket>(packetList, quicPacket, transportLayerPacket)) {
        std::shared_ptr<PacketParser::Packets::TlsRecordPacket::HandshakePacket> handshake;
        if (quicPacket->TryGetTlsHandshakePacket(handshake)) {
            ExtractHandshakeData(handshake, sourceHost, destinationHost, transportLayerPacket);
        } else if (quicPacket->Initial() != nullptr) {
            QString streamID = quicPacket->Initial()->GetStreamIdentifier();
            
            if (!streamInitials.contains(streamID)) {
                streamInitials.insert(streamID, { quicPacket->Initial() });
            } else {
                QList<std::shared_ptr<PacketParser::Packets::QuicPacket::InitialPacket>>& initials = streamInitials[streamID];
                initials.append(quicPacket->Initial());
                
                if (PacketParser::Packets::QuicPacket::TryGetTlsHandshakePacket(quicPacket->ParentFrame(), initials, handshake)) {
                    ExtractHandshakeData(handshake, sourceHost, destinationHost, transportLayerPacket);
                    streamInitials.remove(streamID);
                } else if (initials.size() > 10) {
                    streamInitials.remove(streamID);
                }
            }
        }
    }
}

void QuicPacketHandler::ExtractHandshakeData(std::shared_ptr<PacketParser::Packets::TlsRecordPacket::HandshakePacket> handshake, NetworkHost* sourceHost, NetworkHost* destinationHost, std::shared_ptr<PacketParser::ITransportLayerPacket> transportLayerPacket, bool transferIsClientToServer) {
    PacketParser::FiveTuple fiveTuple;
    if (transportLayerPacket->DestinationPort() == 443 || transportLayerPacket->DestinationPort() < transportLayerPacket->SourcePort()) {
        fiveTuple = PacketParser::FiveTuple(sourceHost, transportLayerPacket->SourcePort(), destinationHost, transportLayerPacket->DestinationPort(), static_cast<PacketParser::FiveTuple::TransportProtocol>(transportLayerPacket->TransportProtocol()));
    } else {
        fiveTuple = PacketParser::FiveTuple(destinationHost, transportLayerPacket->DestinationPort(), sourceHost, transportLayerPacket->SourcePort(), static_cast<PacketParser::FiveTuple::TransportProtocol>(transportLayerPacket->TransportProtocol()));
    }
    
    AbstractTlsHandshakePacketHandler::ExtractHandshakeData(transportLayerPacket, fiveTuple, transferIsClientToServer, handshake);
}

void QuicPacketHandler::Reset() {
    streamInitials.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
