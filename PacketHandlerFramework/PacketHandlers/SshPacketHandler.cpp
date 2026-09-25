#include "SshPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SshPacketHandler::SshPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SshPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SshPacket>() };
}

ApplicationLayerProtocol SshPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::SSH;
}

int SshPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    for (const auto& p : packetList) {
        if (auto sshPacket = std::dynamic_pointer_cast<PacketParser::Packets::SshPacket>(p)) {
            NetworkHost* sender;
            if (transferIsClientToServer) {
                sender = tcpSession->GetClientHost();
            } else {
                sender = tcpSession->GetServerHost();
            }

            sender->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SshVersion, sshPacket->SshVersion());
            sender->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SshApplication, sshPacket->SshApplication());

            return p->PacketLength();
        }
    }
    return 0;
}

void SshPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
