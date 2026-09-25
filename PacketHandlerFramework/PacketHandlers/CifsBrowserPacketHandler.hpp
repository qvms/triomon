#pragma once

#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class CifsBrowserPacketHandler : public AbstractPacketHandler { // Also conceptually implements IPacketHandler
public:
    CifsBrowserPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~CifsBrowserPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override; // Stub override

    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
