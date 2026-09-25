#include <cstdint>
#include "PcapOverIpReceiverFactory.hpp"

namespace NetworkMiner {
namespace PacketStream {

uint16_t PcapOverIpReceiverFactory::nextPortNumber = 57012;

void PcapOverIpReceiverFactory::GetPcapOverIp(std::shared_ptr<PacketHandlerFramework::MainPacketHandler> packetHandler, void* addCaseFileCallback, void* caseFileLoadedCallback, void* completedEventHandler, void* owner) {
    // Stub implementation to show ReceivePcapOverTcp
}

} // namespace PacketStream
} // namespace NetworkMiner
