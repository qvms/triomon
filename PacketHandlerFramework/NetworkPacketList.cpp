#include "NetworkPacketList.hpp"
#include <climits>

namespace PacketHandlerFramework {

NetworkPacketList::NetworkPacketList() : totalBytes(0), packetCount(0) {
    // Max DateTime equivalent
    firstSeen = QDateTime(QDate(9999, 12, 31), QTime(23, 59, 59)); 
}

int NetworkPacketList::Count() const {
    std::lock_guard<std::mutex> lock(listMutex);
    return packetCount;
}

long long NetworkPacketList::TotalBytes() const {
    std::lock_guard<std::mutex> lock(listMutex);
    return totalBytes;
}

QDateTime NetworkPacketList::FirstPacketTimestamp() const {
    std::lock_guard<std::mutex> lock(listMutex);
    return firstSeen;
}

void NetworkPacketList::AddRange(const std::vector<std::shared_ptr<NetworkPacket>>& collection) {
    std::lock_guard<std::mutex> lock(listMutex);
    for (const auto& p : collection) {
        // internal logic w/o taking lock again
        if (p) {
            packetCount++;
            totalBytes += p->PacketBytes();
            if (p->Timestamp() < firstSeen) {
                firstSeen = p->Timestamp();
            }
        }
    }
}

void NetworkPacketList::Add(std::shared_ptr<NetworkPacket> packet) {
    if (!packet) return;
    std::lock_guard<std::mutex> lock(listMutex);
    packetCount++;
    totalBytes += packet->PacketBytes();
    if (packet->Timestamp() < firstSeen) {
        firstSeen = packet->Timestamp();
    }
}

} // namespace PacketHandlerFramework
