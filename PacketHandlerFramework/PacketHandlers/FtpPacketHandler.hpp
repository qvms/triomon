#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/FtpPacket.hpp"
#include "../../PacketParser/Packets/TcpPacket.hpp"
#include "../../PacketParser/NetworkTcpSession.hpp"
#include "../../PacketParser/NetworkHost.hpp"
#include "../../PacketParser/PopularityList.hpp"
#include <QString>
#include <memory>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class FtpPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
private:
    class FtpSession;
    class PendingFileTransfer {
    public:
        std::shared_ptr<PacketParser::NetworkHost> dataSessionClient;
        std::shared_ptr<PacketParser::NetworkHost> dataSessionServer;
        int dataSessionClientPort; // -1 if unknown
        uint16_t dataSessionServerPort;
        bool dataSessionIsPassive;
        int fileDirectionIsDataSessionServerToDataSessionClient; // -1 unknown, 0 false, 1 true
        bool fileTransferSessionEstablished;
        std::shared_ptr<FtpSession> ftpControlSession;
        QString filename;
        QString details;

        PendingFileTransfer(std::shared_ptr<PacketParser::NetworkHost> client, int clientPort, std::shared_ptr<PacketParser::NetworkHost> server, uint16_t serverPort, bool isPassive, std::shared_ptr<FtpSession> controlSession);
        
        static QString GetKey(std::shared_ptr<PacketParser::NetworkHost> client, int clientPort, std::shared_ptr<PacketParser::NetworkHost> server, int serverPort);
        QString GetKey() const;
    };

    class FtpSession : public std::enable_shared_from_this<FtpSession> {
    public:
        std::shared_ptr<PacketParser::NetworkHost> ftpClient;
        std::shared_ptr<PacketParser::NetworkHost> ftpServer;
        QString username;
        QString password;
        std::shared_ptr<PendingFileTransfer> pendingFileTransfer;

        FtpSession(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server);
    };

    PacketParser::PopularityList<std::shared_ptr<PacketParser::NetworkTcpSession>, std::shared_ptr<FtpSession>> ftpSessionList;
    PacketParser::PopularityList<QString, std::shared_ptr<PendingFileTransfer>> pendingFileTransferList;

    bool TryGetPort(const QString& commaSeparatedIpAndPortString, uint16_t& portNumber);

public:
    FtpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~FtpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    
    // ITcpSessionPacketHandler
    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset();

    bool TryExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
