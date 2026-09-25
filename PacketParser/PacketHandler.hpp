#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QByteArray>
#include <QDateTime>
#include <memory>
#include <vector>

#include "Packets/AbstractPacket.hpp"
#include "PacketParser/Frame.hpp"
#include "Packets/TcpPacket.hpp"
#include "Packets/UdpPacket.hpp"
#include "Packets/IPv4Packet.hpp"
#include "Packets/IPv6Packet.hpp"
#include "Fingerprints/IOsFingerprinter.hpp"
#include "PacketHandlerFramework/NetworkHostList.hpp"
#include "PacketParser/NetworkTcpSession.hpp"
#include "PacketHandlerFramework/FileTransfer/FileStreamAssemblerList.hpp"
#include "PacketHandlerFramework/NetworkCredential.hpp"
#include "PacketParser/NetworkHost.hpp"

namespace PacketParser {

struct ParsedSession {
    QString clientIp;
    QString serverIp;
    int clientPort;
    int serverPort;
    QString protocol;
    int frameLength;
    QDateTime timestamp;
};

class PacketHandler {
private:
    std::shared_ptr<PacketHandlerFramework::NetworkHostList> networkHostList;
    std::vector<ParsedSession> sessions;
    int nFramesReceived;
    int nBytesReceived;
public:
    PacketHandler();
    virtual ~PacketHandler();

    std::shared_ptr<PacketHandlerFramework::NetworkHostList> GetNetworkHostList() const;
    const std::vector<ParsedSession>& GetSessions() const { return sessions; }

    void OnAnomalyDetected(const QString& description);
    
    // Stubs to allow compilation for things depending on it
    std::shared_ptr<NetworkTcpSession> GetNetworkTcpSession(Packets::TcpPacket* tcpPacket, std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost) { return nullptr; }
    
    void ParseFrame(std::shared_ptr<PacketParser::Frame> frame);
};

} // namespace PacketParser
