#pragma once

#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class NtlmSspPacketHandler : public AbstractPacketHandler {
public:
    NtlmSspPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~NtlmSspPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
