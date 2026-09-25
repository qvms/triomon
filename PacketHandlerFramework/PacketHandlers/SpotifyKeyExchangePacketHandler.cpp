#include "SpotifyKeyExchangePacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SpotifyKeyExchangePacketHandler::SpotifyKeyExchangePacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SpotifyKeyExchangePacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SpotifyKeyExchangePacket>() };
}

ApplicationLayerProtocol SpotifyKeyExchangePacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::Spotify;
}

int SpotifyKeyExchangePacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::SpotifyKeyExchangePacket> spotifyPacket;
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;

    for (const auto& p : packetList) {
        if (auto tcp = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p)) {
            tcpPacket = tcp;
        } else if (auto spotify = std::dynamic_pointer_cast<PacketParser::Packets::SpotifyKeyExchangePacket>(p)) {
            spotifyPacket = spotify;
        }
    }

    if (spotifyPacket && tcpPacket) {
        QMap<QString, QString> tmpCol;
        
        if (spotifyPacket->IsClientGreeting()) {
            if (spotifyPacket->ClientNonce().length() > 0) {
                QString nonceString = spotifyPacket->ClientNonce().toHex().toUpper();
                tmpCol.insert("Client Nonce", nonceString);
            }
        }
        
        if (tmpCol.size() > 0) {
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                spotifyPacket->ParentFrame()->GetFrameNumber(),
                tcpSession->GetFlow()->FiveTuple(),
                transferIsClientToServer,
                tmpCol,
                spotifyPacket->ParentFrame()->GetTimestamp(),
                "Spotify Key Exchange"
            ));
        }

        return spotifyPacket->ParsedBytesCount();
    }
    
    return 0;
}

void SpotifyKeyExchangePacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
