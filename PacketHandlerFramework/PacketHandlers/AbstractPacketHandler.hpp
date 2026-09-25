#pragma once

#include <memory>
#include <QString>

namespace PacketParser {
class ITcpFlowInfo; // Forward declaration
namespace Packets {
class AbstractPacket; // Forward declaration
}
}

namespace PacketHandlerFramework {

class MainPacketHandler; // Forward declaration

namespace PacketHandlers {

class AbstractPacketHandler {
protected:
    std::shared_ptr<MainPacketHandler> mainPacketHandler;

public:
    AbstractPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~AbstractPacketHandler() = default;

    virtual void ExtractDataFromPacket(void* transportPacket) = 0; // Void* for AbstractPacket 
    
    std::shared_ptr<MainPacketHandler> GetMainPacketHandler() const { return mainPacketHandler; }

    bool TryGetPacketSender(void* packet, QString& sender);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
