#pragma once

#include "ITcpFlowInfo.hpp"
#include <QHostAddress>
#include <cstdint>
#include <optional>

namespace PacketParser {

class TcpFlowInfo : public ITcpFlowInfo {
private:
    uint16_t m_clientPort;
    uint16_t m_serverPort;
    int64_t m_bytesSentClient;
    int64_t m_bytesSentServer;
    std::optional<QHostAddress> m_clientIP;
    std::optional<QHostAddress> m_serverIP;

public:
    TcpFlowInfo(uint16_t clientPort, uint16_t serverPort);
    TcpFlowInfo(const QHostAddress& clientAddress, uint16_t clientPort, const QHostAddress& serverAddress, uint16_t serverPort);

    uint16_t getClientPort() const override;
    uint16_t getServerPort() const override;

    int64_t getBytesSentClient() const override;
    void setBytesSentClient(int64_t bytes) override;

    int64_t getBytesSentServer() const override;
    void setBytesSentServer(int64_t bytes) override;

    std::optional<QHostAddress> getClientIP() const;
    std::optional<QHostAddress> getServerIP() const;
};

} // namespace PacketParser
