#include "TabularDataStreamPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

TabularDataStreamPacketHandler::TabularDataStreamPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

int TabularDataStreamPacketHandler::ExtractData(void* tcpSession, bool transferIsClientToServer, const std::vector<std::shared_ptr<void>>& packetList) {
    // Stub
    return 0;
}

void TabularDataStreamPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
