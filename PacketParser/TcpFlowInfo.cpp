#include "TcpFlowInfo.hpp"

namespace PacketParser {

TcpFlowInfo::TcpFlowInfo(uint16_t clientPort, uint16_t serverPort)
    : m_clientPort(clientPort)
    , m_serverPort(serverPort)
    , m_bytesSentClient(0)
    , m_bytesSentServer(0)
    , m_clientIP(std::nullopt)
    , m_serverIP(std::nullopt)
{
}

TcpFlowInfo::TcpFlowInfo(const QHostAddress& clientAddress, uint16_t clientPort, const QHostAddress& serverAddress, uint16_t serverPort)
    : m_clientPort(clientPort)
    , m_serverPort(serverPort)
    , m_bytesSentClient(0)
    , m_bytesSentServer(0)
    , m_clientIP(clientAddress)
    , m_serverIP(serverAddress)
{
}

uint16_t TcpFlowInfo::getClientPort() const {
    return m_clientPort;
}

uint16_t TcpFlowInfo::getServerPort() const {
    return m_serverPort;
}

int64_t TcpFlowInfo::getBytesSentClient() const {
    return m_bytesSentClient;
}

void TcpFlowInfo::setBytesSentClient(int64_t bytes) {
    m_bytesSentClient = bytes;
}

int64_t TcpFlowInfo::getBytesSentServer() const {
    return m_bytesSentServer;
}

void TcpFlowInfo::setBytesSentServer(int64_t bytes) {
    m_bytesSentServer = bytes;
}

std::optional<QHostAddress> TcpFlowInfo::getClientIP() const {
    return m_clientIP;
}

std::optional<QHostAddress> TcpFlowInfo::getServerIP() const {
    return m_serverIP;
}

} // namespace PacketParser
