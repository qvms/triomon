#include <cstdint>
#pragma once
#include <memory>
#include <functional>

namespace PacketHandlerFramework {
class MainPacketHandler;
}

namespace NetworkMiner {
namespace PacketStream {

class PcapOverIpReceiverFactory {
private:
    static uint16_t nextPortNumber;

public:
    static void GetPcapOverIp(std::shared_ptr<PacketHandlerFramework::MainPacketHandler> packetHandler, void* addCaseFileCallback, void* caseFileLoadedCallback, void* completedEventHandler, void* owner);
};

} // namespace PacketStream
} // namespace NetworkMiner
