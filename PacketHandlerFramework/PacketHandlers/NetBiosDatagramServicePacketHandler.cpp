#include "NetBiosDatagramServicePacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

NetBiosDatagramServicePacketHandler::NetBiosDatagramServicePacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> NetBiosDatagramServicePacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::NetBiosDatagramServicePacket>() };
}

ApplicationLayerProtocol NetBiosDatagramServicePacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::NetBiosDatagramService;
}

void NetBiosDatagramServicePacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::NetBiosDatagramServicePacket> nbdsPacket;
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;

    for (const auto& p : packetList) {
        if (auto nbds = std::dynamic_pointer_cast<PacketParser::Packets::NetBiosDatagramServicePacket>(p)) {
            nbdsPacket = nbds;
        } else if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        }
    }

    if (nbdsPacket && udpPacket) {
        if (!nbdsPacket->SourceName().isEmpty()) {
            sourceHost->AddHostName(nbdsPacket->SourceName(), "NBDS Source");
        }
        if (!nbdsPacket->DestinationName().isEmpty()) {
            destinationHost->AddHostName(nbdsPacket->DestinationName(), "NBDS Destination");
        }

        QMap<QString, QString> tmpCol;
        if (!nbdsPacket->SourceName().isEmpty()) tmpCol.insert("Source Name", nbdsPacket->SourceName());
        if (!nbdsPacket->DestinationName().isEmpty()) tmpCol.insert("Destination Name", nbdsPacket->DestinationName());
        
        if (nbdsPacket->PacketType() == 0x11) {
            tmpCol.insert("Packet Type", "DIRECT_GROUP");
        } else if (nbdsPacket->PacketType() == 0x12) {
            tmpCol.insert("Packet Type", "BROADCAST");
        } else if (nbdsPacket->PacketType() == 0x13) {
            tmpCol.insert("Packet Type", "ERROR");
        } else if (nbdsPacket->PacketType() == 0x14) {
            tmpCol.insert("Packet Type", "DATAGRAM QUERY REQUEST");
        } else if (nbdsPacket->PacketType() == 0x15) {
            tmpCol.insert("Packet Type", "DATAGRAM POSITIVE QUERY RESPONSE");
        } else if (nbdsPacket->PacketType() == 0x16) {
            tmpCol.insert("Packet Type", "DATAGRAM NEGATIVE QUERY RESPONSE");
        }
        
        if (tmpCol.size() > 0) {
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                nbdsPacket->ParentFrame()->GetFrameNumber(),
                sourceHost, destinationHost, udpPacket->TransportProtocol(),
                udpPacket->SourcePort(), udpPacket->DestinationPort(),
                tmpCol, nbdsPacket->ParentFrame()->GetTimestamp(), "NetBIOS Datagram Service"
            ));
        }
    }
}

void NetBiosDatagramServicePacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
