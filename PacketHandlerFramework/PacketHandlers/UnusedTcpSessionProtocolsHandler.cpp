#include "UnusedTcpSessionProtocolsHandler.hpp"
#include "../PacketParser/Packets/NetBiosDatagramServicePacket.hpp"
#include "../PacketParser/Packets/NetBiosNameServicePacket.hpp"
#include "../PacketParser/Packets/AbstractPacket.hpp"
#include "../PacketParser/Frame.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

UnusedTcpSessionProtocolsHandler::UnusedTcpSessionProtocolsHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> UnusedTcpSessionProtocolsHandler::ParsedTypes() const {
    return {
        qMetaTypeId<PacketParser::Packets::NetBiosDatagramServicePacket>(),
        qMetaTypeId<PacketParser::Packets::NetBiosNameServicePacket>()
    };
}

PacketParser::ApplicationLayerProtocol UnusedTcpSessionProtocolsHandler::HandledProtocol() const {
    return PacketParser::ApplicationLayerProtocol::Unknown;
}

int UnusedTcpSessionProtocolsHandler::ExtractData(NetworkTcpSession* /*tcpSession*/, bool /*transferIsClientToServer*/, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    QList<int> types = ParsedTypes();
    
    for (const auto& p : packetList) {
        if (!p) continue;
        
        int typeId = -1;
        if (std::dynamic_pointer_cast<PacketParser::Packets::NetBiosDatagramServicePacket>(p)) {
            typeId = qMetaTypeId<PacketParser::Packets::NetBiosDatagramServicePacket>();
        } else if (std::dynamic_pointer_cast<PacketParser::Packets::NetBiosNameServicePacket>(p)) {
            typeId = qMetaTypeId<PacketParser::Packets::NetBiosNameServicePacket>();
        }
        
        if (types.contains(typeId)) {
            if (p->ParentFrame()) {
                return p->ParentFrame()->DataLength();
            }
        }
    }

    return 0;
}

void UnusedTcpSessionProtocolsHandler::Reset() {
    // do nothing since this one holds no state
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
