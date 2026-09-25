#include "CifsBrowserPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

CifsBrowserPacketHandler::CifsBrowserPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void CifsBrowserPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Stub
}

void CifsBrowserPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) {
    // Stub: Without CifsBrowserPacket definition, we can't fully parse
}

void CifsBrowserPacketHandler::Reset() {
    // no state
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
