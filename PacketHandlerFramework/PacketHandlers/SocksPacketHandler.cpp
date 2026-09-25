#include "SocksPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SocksPacketHandler::SocksPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SocksPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SocksPacket>() };
}

ApplicationLayerProtocol SocksPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::SOCKS;
}

int SocksPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::SocksPacket> socksPacket;
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;

    for (const auto& p : packetList) {
        if (auto tcp = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p)) {
            tcpPacket = tcp;
        } else if (auto socks = std::dynamic_pointer_cast<PacketParser::Packets::SocksPacket>(p)) {
            socksPacket = socks;
        }
    }

    if (socksPacket && tcpPacket) {
        QString paramName = "SOCKS";
        if (socksPacket->ClientToServer()) {
            if (socksPacket->CommandOrReply() == 1) {
                paramName += " Connect";
                if (!socksConnectIpPorts.contains(tcpSession)) {
                    if (!socksPacket->IpAddress().isNull()) {
                        socksConnectIpPorts.insert(tcpSession, qMakePair(socksPacket->IpAddress(), socksPacket->Port()));
                    } else {
                        socksConnectIpPorts.insert(tcpSession, qMakePair(tcpSession->GetClientHost()->IPAddress(), socksPacket->Port()));
                    }
                }
            } else if (socksPacket->CommandOrReply() == 2) {
                paramName += " Bind";
            } else if (socksPacket->CommandOrReply() == 3) {
                paramName += " DNS Associate";
            }

            if (!socksPacket->Username().isNull() && !socksPacket->Password().isNull()) {
                MainPacketHandler()->AddCredential(std::make_shared<NetworkCredential>(
                    tcpSession->GetClientHost(), tcpSession->GetServerHost(), "SOCKS",
                    socksPacket->Username(), socksPacket->Password(), socksPacket->ParentFrame()->GetTimestamp()));
            }
        } else {
            if (socksPacket->CommandOrReply() == 0) {
                paramName += " Bind Succeeded";
                if (socksConnectIpPorts.contains(tcpSession)) {
                    QPair<QHostAddress, quint16> target = socksConnectIpPorts[tcpSession];
                    quint16 serverPort = target.second;
                    NetworkHost* serverHost;
                    if (MainPacketHandler()->NetworkHostList()->ContainsIP(target.first)) {
                        serverHost = MainPacketHandler()->NetworkHostList()->GetNetworkHost(target.first);
                    } else {
                        serverHost = tcpSession->GetClientHost();
                    }

                    tcpSession->SetProtocolFinder(std::make_shared<PacketParser::TcpSessionProtocolFinder>(
                        tcpSession->GetFlow(), tcpPacket->ParentFrame()->GetFrameNumber(),
                        MainPacketHandler(), serverHost, serverPort));
                }
            } else if (socksPacket->CommandOrReply() == 1) {
                paramName += " General SOCKS server failure";
            } else if (socksPacket->CommandOrReply() == 2) {
                paramName += " Connection not allowed by ruleset";
            } else if (socksPacket->CommandOrReply() == 3) {
                paramName += " Network unreachable";
            } else if (socksPacket->CommandOrReply() == 4) {
                paramName += " Host unreachable";
            } else if (socksPacket->CommandOrReply() == 5) {
                paramName += " Connection refused";
            } else if (socksPacket->CommandOrReply() == 6) {
                paramName += " TTL expired";
            } else if (socksPacket->CommandOrReply() == 7) {
                paramName += " Command not supported";
            } else if (socksPacket->CommandOrReply() == 8) {
                paramName += " Address type not supported";
            }
        }

        if (socksPacket->ATyp() == PacketParser::Packets::SocksPacket::ATYP::DOMAINNAME) {
            QMap<QString, QString> parms;
            parms.insert(paramName, socksPacket->DomainName() + ":" + QString::number(socksPacket->Port()));
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                socksPacket->ParentFrame()->GetFrameNumber(), tcpSession->GetFlow()->FiveTuple(),
                transferIsClientToServer, parms, socksPacket->ParentFrame()->GetTimestamp(), "SOCKS Connection"));
        } else if (socksPacket->ATyp() == PacketParser::Packets::SocksPacket::ATYP::IPv4 || socksPacket->ATyp() == PacketParser::Packets::SocksPacket::ATYP::IPv6) {
            QMap<QString, QString> parms;
            parms.insert(paramName, socksPacket->IpAddress().toString() + ":" + QString::number(socksPacket->Port()));
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                socksPacket->ParentFrame()->GetFrameNumber(), tcpSession->GetFlow()->FiveTuple(),
                transferIsClientToServer, parms, socksPacket->ParentFrame()->GetTimestamp(), "SOCKS Connection"));
        }
    }
    
    if (socksPacket) {
        return socksPacket->ParsedBytesCount();
    }
    return 0;
}

void SocksPacketHandler::Reset() {
    socksConnectIpPorts.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
