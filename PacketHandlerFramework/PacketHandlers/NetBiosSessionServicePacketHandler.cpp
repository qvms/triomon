#include "NetBiosSessionServicePacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

NetBiosSessionServicePacketHandler::NetBiosSessionServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void NetBiosSessionServicePacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // do nothing
}

int NetBiosSessionServicePacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int bytesParsed = 0;
    for (const auto& p : packetList) {
        auto netBiosPacket = std::dynamic_pointer_cast<PacketParser::Packets::NetBiosSessionService>(p);
        if (netBiosPacket) {
            bytesParsed += netBiosPacket->ParsedBytesCount();
        }
    }
    return bytesParsed;
}

void NetBiosSessionServicePacketHandler::Reset() {
    // do nothing
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
