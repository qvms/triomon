#include "FtpPacketHandler.hpp"
#include "../../PacketHandlerFramework/Events/ParametersEventArgs.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include "../../PacketHandlerFramework/FileTransfer/FileStreamAssembler.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"
#include <QVariantMap>
#include <QStringList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

FtpPacketHandler::PendingFileTransfer::PendingFileTransfer(std::shared_ptr<PacketParser::NetworkHost> client, int clientPort, std::shared_ptr<PacketParser::NetworkHost> server, uint16_t serverPort, bool isPassive, std::shared_ptr<FtpSession> controlSession) 
    : dataSessionClient(client), dataSessionClientPort(clientPort), dataSessionServer(server), dataSessionServerPort(serverPort), 
      dataSessionIsPassive(isPassive), fileDirectionIsDataSessionServerToDataSessionClient(-1), fileTransferSessionEstablished(false),
      ftpControlSession(controlSession) {}

QString FtpPacketHandler::PendingFileTransfer::GetKey(std::shared_ptr<PacketParser::NetworkHost> client, int clientPort, std::shared_ptr<PacketParser::NetworkHost> server, int serverPort) {
    return QString("Data session client : %1 TCP/%2\nData session server : %3 TCP/%4")
        .arg(client->IPAddress().toString()).arg(clientPort >= 0 ? QString::number(clientPort) : "")
        .arg(server->IPAddress().toString()).arg(serverPort >= 0 ? QString::number(serverPort) : "");
}

QString FtpPacketHandler::PendingFileTransfer::GetKey() const {
    return GetKey(dataSessionClient, dataSessionClientPort, dataSessionServer, dataSessionServerPort);
}

FtpPacketHandler::FtpSession::FtpSession(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server)
    : ftpClient(client), ftpServer(server) {}

FtpPacketHandler::FtpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler), ftpSessionList(100), pendingFileTransferList(20) {
}

void FtpPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // ITcpSessionPacketHandler
}

int FtpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::NetworkHost> sourceHost = transferIsClientToServer ? tcpSession->GetClientHost() : tcpSession->GetServerHost();
    std::shared_ptr<PacketParser::NetworkHost> destinationHost = transferIsClientToServer ? tcpSession->GetServerHost() : tcpSession->GetClientHost();
    
    if (TryExtractData(tcpSession, sourceHost, destinationHost, packetList)) {
        int parsedBytes = 0;
        for (auto p : packetList) {
            auto fp = std::dynamic_pointer_cast<PacketParser::Packets::FtpPacket>(p);
            if(fp) {
                parsedBytes += fp->GetPacketLength();
            }
        }
        return parsedBytes;
    }
    return 0;
}

bool FtpPacketHandler::TryExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;
    std::shared_ptr<PacketParser::Packets::FtpPacket> ftpPacket;

    for(auto& p : packetList) {
        if(!tcpPacket) tcpPacket = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p);
        if(!ftpPacket) ftpPacket = std::dynamic_pointer_cast<PacketParser::Packets::FtpPacket>(p);
    }

    std::shared_ptr<FtpSession> ftpSession;
    bool returnValue = false;

    if(tcpSession->SynPacketReceived() && tcpSession->SynAckPacketReceived() && !tcpSession->SessionEstablished()) {
        QString key1 = PendingFileTransfer::GetKey(tcpSession->GetClientHost(), tcpSession->ClientTcpPort(), tcpSession->GetServerHost(), tcpSession->ServerTcpPort());
        if(pendingFileTransferList.ContainsKey(key1)) {
            auto pending = pendingFileTransferList.GetValue(key1);
            pending->fileTransferSessionEstablished = true;
            ftpSession = pending->ftpControlSession;
            returnValue = true;
        } else {
            QString key2 = PendingFileTransfer::GetKey(tcpSession->GetClientHost(), -1, tcpSession->GetServerHost(), tcpSession->ServerTcpPort());
            if(pendingFileTransferList.ContainsKey(key2)) {
                auto pending = pendingFileTransferList.GetValue(key2);
                pendingFileTransferList.Remove(pending->GetKey());
                pending->dataSessionClientPort = tcpSession->ClientTcpPort();
                pending->fileTransferSessionEstablished = true;
                pendingFileTransferList.Add(pending->GetKey(), pending);
                ftpSession = pending->ftpControlSession;
                returnValue = true;
            }
        }
    }

    if(ftpPacket && tcpPacket) {
        returnValue = true;
        if(ftpSessionList.ContainsKey(tcpSession)) {
            ftpSession = ftpSessionList.GetValue(tcpSession);
        } else {
            ftpSession = std::make_shared<FtpSession>(tcpSession->GetClientHost(), tcpSession->GetServerHost());
            ftpSessionList.Add(tcpSession, ftpSession);
        }

        if(ftpPacket->ClientToServer()) {
            if(!ftpPacket->RequestCommand().isEmpty() && !ftpPacket->RequestArgument().isEmpty()) {
                QVariantMap parms;
                parms.insert(ftpPacket->RequestCommand(), ftpPacket->RequestArgument());
                auto ea = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                    ftpPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost, 
                    "TCP " + QString::number(tcpPacket->SourcePort()), "TCP " + QString::number(tcpPacket->DestinationPort()), 
                    parms, ftpPacket->GetParentFrame()->GetTimestamp(), "FTP Request");
                GetMainPacketHandler()->OnParametersDetected(ea);
            }
            if (ftpPacket->RequestCommand() == "USER") {
                ftpSession->username = ftpPacket->RequestArgument();
            } else if (ftpPacket->RequestCommand() == "PASS") {
                ftpSession->password = ftpPacket->RequestArgument();
                if(!ftpSession->username.isEmpty() && !ftpSession->password.isEmpty()) {
                    auto cred = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                        tcpSession->GetClientHost(), tcpSession->GetServerHost(), 
                        ftpPacket->GetPacketTypeDescription(), ftpSession->username, ftpSession->password, 
                        ftpPacket->GetParentFrame()->GetTimestamp());
                    GetMainPacketHandler()->AddCredential(cred);
                }
            } else if (ftpPacket->RequestCommand() == "PORT") {
                uint16_t clientListeningOnPort;
                if(TryGetPort(ftpPacket->RequestArgument(), clientListeningOnPort)) {
                    ftpSession->pendingFileTransfer = std::make_shared<PendingFileTransfer>(
                        ftpSession->ftpServer, -1, ftpSession->ftpClient, clientListeningOnPort, false, ftpSession);
                    if(pendingFileTransferList.ContainsKey(ftpSession->pendingFileTransfer->GetKey())) {
                        pendingFileTransferList.Remove(ftpSession->pendingFileTransfer->GetKey());
                    }
                    pendingFileTransferList.Add(ftpSession->pendingFileTransfer->GetKey(), ftpSession->pendingFileTransfer);
                }
            } else if (ftpPacket->RequestCommand() == "STOR") {
                if(ftpSession->pendingFileTransfer) {
                    ftpSession->pendingFileTransfer->filename = ftpPacket->RequestArgument();
                    ftpSession->pendingFileTransfer->fileDirectionIsDataSessionServerToDataSessionClient = !ftpSession->pendingFileTransfer->dataSessionIsPassive ? 1 : 0;
                    ftpSession->pendingFileTransfer->details = ftpPacket->RequestCommand() + " " + ftpPacket->RequestArgument();
                } else {
                    GetMainPacketHandler()->OnAnomalyDetected("STOR command without a pending ftp data session. Frame: " /* + ftpPacket->GetParentFrame()->ToString() */);
                }
            } else if (ftpPacket->RequestCommand() == "RETR") {
                if(ftpSession->pendingFileTransfer) {
                    ftpSession->pendingFileTransfer->filename = ftpPacket->RequestArgument();
                    ftpSession->pendingFileTransfer->fileDirectionIsDataSessionServerToDataSessionClient = ftpSession->pendingFileTransfer->dataSessionIsPassive ? 1 : 0;
                    ftpSession->pendingFileTransfer->details = ftpPacket->RequestCommand() + " " + ftpPacket->RequestArgument();
                } else {
                    GetMainPacketHandler()->OnAnomalyDetected("RETR command without a pending ftp data session. Frame: " /* + ftpPacket->GetParentFrame()->ToString() */);
                }
            }
        } else {
            if (ftpPacket->ResponseCode() != 0 && !ftpPacket->ResponseArgument().isEmpty()) {
                QVariantMap parms;
                parms.insert(QString::number(ftpPacket->ResponseCode()), ftpPacket->ResponseArgument());
                auto ea = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                    ftpPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost, 
                    "TCP " + QString::number(tcpPacket->SourcePort()), "TCP " + QString::number(tcpPacket->DestinationPort()), 
                    parms, ftpPacket->GetParentFrame()->GetTimestamp(), "FTP Response");
                GetMainPacketHandler()->OnParametersDetected(ea);
                
                if (ftpPacket->ResponseCode() == 220 && ftpPacket->ResponseArgument().toLower().contains("ftp")) {
                    sourceHost->AddFtpServerBanner(ftpPacket->ResponseArgument(), tcpPacket->SourcePort());
                }
            }
            if (ftpPacket->ResponseCode() == 226) {
            } else if (ftpPacket->ResponseCode() == 227) {
                QString ipAndPort = ftpPacket->ResponseArgument();
                int startIndex = ipAndPort.indexOf(QRegularExpression("[0-9]"));
                if (startIndex != -1) {
                    ipAndPort = ipAndPort.mid(startIndex);
                    int endIndex = -1;
                    for (int i = ipAndPort.length() - 1; i >= 0; --i) {
                        if (ipAndPort[i].isDigit()) {
                            endIndex = i;
                            break;
                        }
                    }
                    if (endIndex != -1) {
                        ipAndPort = ipAndPort.mid(0, endIndex + 1);
                        uint16_t serverListeningOnPort;
                        if(TryGetPort(ipAndPort, serverListeningOnPort)) {
                            ftpSession->pendingFileTransfer = std::make_shared<PendingFileTransfer>(
                                ftpSession->ftpClient, -1, ftpSession->ftpServer, serverListeningOnPort, true, ftpSession);
                            if(pendingFileTransferList.ContainsKey(ftpSession->pendingFileTransfer->GetKey())) {
                                pendingFileTransferList.Remove(ftpSession->pendingFileTransfer->GetKey());
                            }
                            pendingFileTransferList.Add(ftpSession->pendingFileTransfer->GetKey(), ftpSession->pendingFileTransfer);
                        }
                    }
                }
            }
            if (ftpPacket->ResponseCode() == 230) {
                if(!ftpSession->username.isEmpty() && !ftpSession->password.isEmpty()) {
                    auto cred = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                        tcpSession->GetClientHost(), tcpSession->GetServerHost(), 
                        ftpPacket->GetPacketTypeDescription(), ftpSession->username, ftpSession->password, 
                        true, ftpPacket->GetParentFrame()->GetTimestamp());
                    GetMainPacketHandler()->AddCredential(cred);
                }
            }
        }
    }

    if (ftpSession && ftpSession->pendingFileTransfer) {
        returnValue = true;
        auto pending = ftpSession->pendingFileTransfer;
        if(pending->fileTransferSessionEstablished && pending->fileDirectionIsDataSessionServerToDataSessionClient != -1 && pending->dataSessionClientPort != -1) {
            std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> assembler;
            if(pending->fileDirectionIsDataSessionServerToDataSessionClient == 1) {
                assembler = std::make_shared<PacketHandlerFramework::FileTransfer::FileStreamAssembler>(
                    GetMainPacketHandler()->GetFileStreamAssemblerList(), pending->dataSessionServer, pending->dataSessionServerPort, 
                    pending->dataSessionClient, pending->dataSessionClientPort, true, PacketHandlerFramework::FileTransfer::FileStreamTypes::FTP, 
                    pending->filename, "/", pending->details, tcpPacket->GetParentFrame()->GetFrameNumber(), tcpPacket->GetParentFrame()->GetTimestamp());
            } else {
                assembler = std::make_shared<PacketHandlerFramework::FileTransfer::FileStreamAssembler>(
                    GetMainPacketHandler()->GetFileStreamAssemblerList(), pending->dataSessionClient, pending->dataSessionClientPort, 
                    pending->dataSessionServer, pending->dataSessionServerPort, true, PacketHandlerFramework::FileTransfer::FileStreamTypes::FTP, 
                    pending->filename, "/", pending->details, tcpPacket->GetParentFrame()->GetFrameNumber(), tcpPacket->GetParentFrame()->GetTimestamp());
            }
            assembler->SetFileContentLength(-1);
            // assembler->SetFileSegmentRemainingBytes(-1);

            if(assembler->TryActivate()) {
                GetMainPacketHandler()->GetFileStreamAssemblerList()->Add(assembler);
            }
            pendingFileTransferList.Remove(pending->GetKey());
            ftpSession->pendingFileTransfer = nullptr;
        }
    }
    
    return returnValue;
}

void FtpPacketHandler::Reset() {
    ftpSessionList.Clear();
    pendingFileTransferList.Clear();
}

bool FtpPacketHandler::TryGetPort(const QString& commaSeparatedIpAndPortString, uint16_t& portNumber) {
    portNumber = 0;
    QStringList data = commaSeparatedIpAndPortString.split(QRegularExpression("[, \\r\\n]"), Qt::SkipEmptyParts);
    if(data.size() < 6) return false;
    uint16_t port = 0;
    for(int i = 4; i < 6; i++) {
        bool ok;
        uint8_t b = data[i].toUShort(&ok);
        if(ok && b <= 255) {
            port = (port << 8) + b;
        } else {
            return false;
        }
    }
    portNumber = port;
    return true;
}

}
}
