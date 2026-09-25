#include "NetworkFlow.hpp"

namespace PacketHandlerFramework {

NetworkFlow::NetworkFlow(void* networkTcpSession) {
    // Stub for NetworkTcpSession dependency
}

NetworkFlow::NetworkFlow(std::shared_ptr<PacketParser::FiveTuple> fiveTuple, const QDateTime& startTime)
    : NetworkFlow(fiveTuple, startTime, startTime, 0, 0) {}

NetworkFlow::NetworkFlow(std::shared_ptr<PacketParser::FiveTuple> fiveTuple, const QDateTime& startTime, const QDateTime& endTime, long long bytesSentClient, long long bytesSentServer)
    : fiveTuple(fiveTuple), startTime(startTime), endTime(endTime), bytesSentClient(bytesSentClient), bytesSentServer(bytesSentServer) {}

uint16_t NetworkFlow::ClientPort() const {
    if (fiveTuple) return fiveTuple->ClientPort();
    return 0;
}

uint16_t NetworkFlow::ServerPort() const {
    if (fiveTuple) return fiveTuple->ServerPort();
    return 0;
}

} // namespace PacketHandlerFramework
