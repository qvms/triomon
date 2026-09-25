#include "NetworkTcpSession.hpp"
#include <sstream>

namespace PacketParser {

NetworkTcpSession::NetworkTcpSession(void* tcpSynPacket, std::shared_ptr<NetworkHost> clientHost, std::shared_ptr<NetworkHost> serverHost)
    : clientHost(clientHost), serverHost(serverHost) {
    // Stub
}

NetworkTcpSession::NetworkTcpSession(std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, void* tcpPacket) {
    // Stub
}

bool NetworkTcpSession::FinPacketReceived() const {
    // Mocked stub logic
    return finPacketReceived; 
}

int NetworkTcpSession::GetHashCode(std::shared_ptr<NetworkHost> clientHost, std::shared_ptr<NetworkHost> serverHost, uint16_t clientTcpPort, uint16_t serverTcpPort) {
    // Basic hash mockup
    int cHash = clientHost->IPAddress().toIPv4Address() ^ clientTcpPort;
    int sHash = serverHost->IPAddress().toIPv4Address() ^ serverTcpPort;
    return cHash ^ (sHash << 16) ^ (sHash >> 16);
}

int NetworkTcpSession::GetHashCode() const {
    return GetHashCode(clientHost, serverHost, clientTcpPort, serverTcpPort);
}

std::string NetworkTcpSession::ToString() const {
    std::stringstream ss;
    if (clientHost && serverHost) {
        ss << clientHost->IPAddress().toString().toStdString() << ":" << clientTcpPort 
           << " -> " << serverHost->IPAddress().toString().toStdString() << ":" << serverTcpPort;
    }
    return ss.str();
}

void NetworkTcpSession::SetEstablished(uint32_t clientSeq, uint32_t serverSeq) {
    sessionEstablished = true;
}

} // namespace PacketParser
