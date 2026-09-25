#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

AbstractPacketHandler::AbstractPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : mainPacketHandler(mainPacketHandler) {}

bool AbstractPacketHandler::TryGetPacketSender(void* packet, QString& sender) {
    // Stub
    return false;
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
