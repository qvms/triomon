#pragma once

#include <vector>
#include <QString>
#include "PacketHandlerFramework/NetworkPacket.hpp"

namespace NetworkMiner {

class LoadingProcess {
public:
    static void InvokePacketReceived(std::shared_ptr<PacketHandlerFramework::NetworkPacket> packet) {
        // Stub: Not directly translating .NET event/delegate
    }

    static void InvokeAddDataToDictionary(const QString& pcapFile, const std::vector<QString>& parameters) {
        // Stub: Not directly translating .NET event/delegate
    }
};

} // namespace NetworkMiner
