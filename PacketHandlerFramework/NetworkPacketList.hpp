#pragma once

#include <QDateTime>
#include <vector>
#include <mutex>
#include "NetworkPacket.hpp"

namespace PacketHandlerFramework {

class NetworkPacketList {
private:
    long long totalBytes;
    int packetCount;
    QDateTime firstSeen;
    mutable std::mutex listMutex;

public:
    NetworkPacketList();

    int Count() const;
    long long TotalBytes() const;
    QDateTime FirstPacketTimestamp() const;

    void AddRange(const std::vector<std::shared_ptr<NetworkPacket>>& collection);
    void Add(std::shared_ptr<NetworkPacket> packet);
};

} // namespace PacketHandlerFramework
