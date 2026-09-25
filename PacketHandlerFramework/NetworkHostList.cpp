#include "NetworkHostList.hpp"

namespace PacketHandlerFramework {

NetworkHostList::NetworkHostList() {}

int NetworkHostList::Count() const {
    std::lock_guard<std::mutex> lock(dictMutex);
    return static_cast<int>(networkHostDictionary.size());
}

std::vector<std::shared_ptr<PacketParser::NetworkHost>> NetworkHostList::Hosts() const {
    std::lock_guard<std::mutex> lock(dictMutex);
    std::vector<std::shared_ptr<PacketParser::NetworkHost>> hosts;
    for (const auto& kvp : networkHostDictionary) {
        hosts.push_back(kvp.second);
    }
    return hosts;
}

void NetworkHostList::Clear() {
    std::lock_guard<std::mutex> lock(dictMutex);
    networkHostDictionary.clear();
}

uint64_t NetworkHostList::ToUInt64(const QHostAddress& ip) {
    // Simplified conversion, for IPv6 this is a simplification
    if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
        return ip.toIPv4Address();
    } else {
        // Fallback for IPv6, need custom hash or full 128 bit struct, using hash for now
        return qHash(ip);
    }
}

bool NetworkHostList::ContainsIP(const QHostAddress& ip) const {
    uint64_t ipULong = ToUInt64(ip);
    std::lock_guard<std::mutex> lock(dictMutex);
    return networkHostDictionary.find(ipULong) != networkHostDictionary.end();
}

void NetworkHostList::Add(std::shared_ptr<PacketParser::NetworkHost> host) {
    if (!host || host->IPAddress().isNull()) return;
    
    std::lock_guard<std::mutex> lock(dictMutex);
    networkHostDictionary[ToUInt64(host->IPAddress())] = host;
}

std::shared_ptr<PacketParser::NetworkHost> NetworkHostList::GetNetworkHost(const QHostAddress& ip) const {
    uint64_t ipULong = ToUInt64(ip);
    std::lock_guard<std::mutex> lock(dictMutex);
    auto it = networkHostDictionary.find(ipULong);
    if (it != networkHostDictionary.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace PacketHandlerFramework
