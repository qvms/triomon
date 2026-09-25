#pragma once

#include <memory>
#include <QString>
#include "PacketHandlerFramework/NetworkHostList.hpp"
#include "PacketHandlerFramework/FileTransfer/FileStreamAssemblerList.hpp"
#include "PacketHandlerFramework/NetworkCredential.hpp"

namespace PacketHandlerFramework {

class MainPacketHandler {
public:
    std::shared_ptr<FileTransfer::FileStreamAssemblerList> FileStreamAssemblerList;
    std::shared_ptr<NetworkHostList> GetNetworkHostList() const { return std::make_shared<NetworkHostList>(); } // Stub

    void OnAnomalyDetected(const QString& description) {} // Stub
    void AddCredential(std::shared_ptr<NetworkCredential> credential) {} // Stub
};

} // namespace PacketHandlerFramework
