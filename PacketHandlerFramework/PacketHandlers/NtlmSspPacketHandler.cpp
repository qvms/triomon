#include "NtlmSspPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

NtlmSspPacketHandler::NtlmSspPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void NtlmSspPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Stub
}

void NtlmSspPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) {
    // Stub
}

void NtlmSspPacketHandler::Reset() {
    // Stub
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
