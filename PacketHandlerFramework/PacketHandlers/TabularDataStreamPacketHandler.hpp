#pragma once
#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class TabularDataStreamPacketHandler : public AbstractPacketHandler {
public:
    TabularDataStreamPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~TabularDataStreamPacketHandler() = default;

    int ExtractData(void* tcpSession, bool transferIsClientToServer, const std::vector<std::shared_ptr<void>>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
