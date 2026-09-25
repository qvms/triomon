#pragma once
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <memory>

// Forward declaration to break the cycle
namespace PacketParser {
    class NetworkHost;
}

namespace PacketHandlerFramework {

class NetworkPacket {
private:
    std::shared_ptr<PacketParser::NetworkHost> sourceHost;
    std::shared_ptr<PacketParser::NetworkHost> destinationHost;
    int dataLinkType; // Simplified
    void* ipPacket;
    int payloadLength;

public:
    NetworkPacket() {}
    NetworkPacket(std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, void* ipPacket)
        : sourceHost(sourceHost), destinationHost(destinationHost), ipPacket(ipPacket) {}

    std::shared_ptr<PacketParser::NetworkHost> SourceHost() const { return sourceHost; }
    std::shared_ptr<PacketParser::NetworkHost> DestinationHost() const { return destinationHost; }
    void* IPPacket() const { return ipPacket; }
    int PayloadLength() const { return payloadLength; }
    int PacketBytes() const { return payloadLength; }
    QDateTime Timestamp() const { return QDateTime(); }
};

}
