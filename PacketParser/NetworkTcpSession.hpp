#pragma once

#include "PacketParser/NetworkHost.hpp"
#include <QDateTime>
#include <memory>
#include <cstdint>

namespace PacketParser {

class TcpDataStream; // Forward declaration

class NetworkTcpSession {
private:
    std::shared_ptr<NetworkHost> clientHost;
    std::shared_ptr<NetworkHost> serverHost;
    uint16_t clientTcpPort;
    uint16_t serverTcpPort;

    QDateTime synPacketTimestamp;
    QDateTime latestPacketTimestamp;

    bool synPacketReceived;
    bool synAckPacketReceived;
    bool finPacketReceived;
    bool sessionEstablished;
    bool sessionClosed;

    uint32_t clientToServerFinPacketSequenceNumber;
    uint32_t serverToClientFinPacketSequenceNumber;

    std::shared_ptr<TcpDataStream> clientToServerTcpDataStream;
    std::shared_ptr<TcpDataStream> serverToClientTcpDataStream;

public:
    NetworkTcpSession(void* tcpSynPacket, std::shared_ptr<NetworkHost> clientHost, std::shared_ptr<NetworkHost> serverHost);
    NetworkTcpSession(std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, void* tcpPacket);

    std::shared_ptr<NetworkHost> ClientHost() const { return clientHost; }
    std::shared_ptr<NetworkHost> ServerHost() const { return serverHost; }
    uint16_t ClientTcpPort() const { return clientTcpPort; }
    uint16_t ServerTcpPort() const { return serverTcpPort; }

    QDateTime SessionStartTimestamp() const { return synPacketTimestamp; }
    QDateTime SessionEndTimestamp() const { return latestPacketTimestamp; }

    bool SynPacketReceived() const { return synPacketReceived; }
    bool SynAckPacketReceived() const { return synAckPacketReceived; }
    bool FinPacketReceived() const;
    bool SessionEstablished() const { return sessionEstablished; }
    bool SessionClosed() const { return sessionClosed; }

    static int GetHashCode(std::shared_ptr<NetworkHost> clientHost, std::shared_ptr<NetworkHost> serverHost, uint16_t clientTcpPort, uint16_t serverTcpPort);
    int GetHashCode() const;
    std::string ToString() const;
    
    void SetEstablished(uint32_t clientSeq, uint32_t serverSeq);
};

} // namespace PacketParser
