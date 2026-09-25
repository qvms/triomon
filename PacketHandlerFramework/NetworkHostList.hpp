#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <vector>
#include "PacketParser/NetworkHost.hpp"
#include <QHostAddress>

namespace PacketHandlerFramework {

class NetworkHostList {
private:
    std::map<uint64_t, std::shared_ptr<PacketParser::NetworkHost>> networkHostDictionary;
    mutable std::mutex dictMutex;

public:
    NetworkHostList();

    int Count() const;
    std::vector<std::shared_ptr<PacketParser::NetworkHost>> Hosts() const;

    void Clear();
    bool ContainsIP(const QHostAddress& ip) const;
    void Add(std::shared_ptr<PacketParser::NetworkHost> host);
    std::shared_ptr<PacketParser::NetworkHost> GetNetworkHost(const QHostAddress& ip) const;
    
    // Helper since we don't have ByteConverter for QHostAddress readily implemented in the context yet
    static uint64_t ToUInt64(const QHostAddress& ip);
};

} // namespace PacketHandlerFramework
