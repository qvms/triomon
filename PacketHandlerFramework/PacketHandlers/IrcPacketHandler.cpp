#include "IrcPacketHandler.hpp"
#include "../PacketHandler.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"
#include <QHostAddress>
#include <QStringList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

std::shared_ptr<NetworkCredential> IrcPacketHandler::IrcSession::GetCredential(NetworkHost* sourceHost, NetworkHost* destinationHost, const QDateTime& timestamp) const {
    QString username = User;
    if (username.isEmpty()) {
        if (!Nick.isEmpty()) {
            username = "N/A (only IRC Nick)";
        } else {
            username = "N/A";
        }
    }
    
    QString password = Pass;
    if (password.isEmpty()) {
        password = "N/A";
    }
    
    return std::make_shared<NetworkCredential>(sourceHost, destinationHost, "IRC", username, password, timestamp);
}

IrcPacketHandler::IrcPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> IrcPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::IrcPacket>() };
}

ApplicationLayerProtocol IrcPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::IRC;
}

int IrcPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::IrcPacket> ircPacket;
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;

    for (const auto& p : packetList) {
        if (auto tcp = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p)) {
            tcpPacket = tcp;
        } else if (auto irc = std::dynamic_pointer_cast<PacketParser::Packets::IrcPacket>(p)) {
            ircPacket = irc;
        }
    }

    if (ircPacket && tcpPacket) {
        QMap<QString, QString> tmpCol;
        NetworkHost* sourceHost = tcpSession->GetSourceHost(transferIsClientToServer);
        NetworkHost* destinationHost = tcpSession->GetDestinationHost(transferIsClientToServer);

        for (const auto& m : ircPacket->GetMessages()) {
            tmpCol.insert(m.Command, m.ToString());

            if (m.Command.compare("USER", Qt::CaseInsensitive) == 0) {
                QStringList parameters = m.Parameters;
                if (parameters.size() > 0) {
                    QString ircUser = parameters[0];
                    std::shared_ptr<IrcSession> ircSession;
                    if (ircSessionList.contains(tcpSession)) {
                        ircSession = ircSessionList[tcpSession];
                    } else {
                        ircSession = std::make_shared<IrcSession>();
                        ircSessionList.insert(tcpSession, ircSession);
                    }
                    ircSession->User = ircUser;
                    sourceHost->AddNumberedExtraDetail("IRC Username", ircUser);
                    MainPacketHandler()->AddCredential(ircSession->GetCredential(sourceHost, destinationHost, ircPacket->ParentFrame()->GetTimestamp()));
                }
                if (parameters.size() > 1) {
                    sourceHost->AddHostName(parameters[1], ircPacket->PacketTypeDescription());
                }
                if (parameters.size() > 2) {
                    destinationHost->AddHostName(parameters[2], ircPacket->PacketTypeDescription());
                }
            } else if (m.Command.compare("NICK", Qt::CaseInsensitive) == 0) {
                if (!m.Parameters.isEmpty()) {
                    QString ircNick = m.Parameters.first();
                    std::shared_ptr<IrcSession> ircSession;
                    if (ircSessionList.contains(tcpSession)) {
                        ircSession = ircSessionList[tcpSession];
                    } else {
                        ircSession = std::make_shared<IrcSession>();
                        ircSessionList.insert(tcpSession, ircSession);
                    }
                    ircSession->Nick = ircNick;
                    sourceHost->AddNumberedExtraDetail("IRC Nick", ircNick);
                    MainPacketHandler()->AddCredential(ircSession->GetCredential(sourceHost, destinationHost, ircPacket->ParentFrame()->GetTimestamp()));
                }
            } else if (m.Command.compare("PASS", Qt::CaseInsensitive) == 0) {
                if (!m.Parameters.isEmpty()) {
                    QString ircPass = m.Parameters.first();
                    std::shared_ptr<IrcSession> ircSession;
                    if (ircSessionList.contains(tcpSession)) {
                        ircSession = ircSessionList[tcpSession];
                    } else {
                        ircSession = std::make_shared<IrcSession>();
                        ircSessionList.insert(tcpSession, ircSession);
                    }
                    ircSession->Pass = ircPass;
                    MainPacketHandler()->AddCredential(ircSession->GetCredential(sourceHost, destinationHost, ircPacket->ParentFrame()->GetTimestamp()));
                }
            } else if (m.Command.compare("PRIVMSG", Qt::CaseInsensitive) == 0) {
                QStringList parameters = m.Parameters;
                if (parameters.size() >= 2) {
                    QMap<QString, QString> attributes;
                    attributes.insert("Command", m.Command);
                    QString from = "";
                    if (!m.Prefix.isEmpty()) {
                        attributes.insert("Prefix", m.Prefix);
                        from = m.Prefix;
                    }
                    for (int i = 0; i < parameters.size(); i++) {
                        QString parm = parameters[i];
                        while(parm.startsWith(CTCP_DELIMITER)) parm.remove(0, 1);
                        while(parm.endsWith(CTCP_DELIMITER)) parm.remove(parm.length()-1, 1);
                        attributes.insert("Parameter " + QString::number(i + 1), parm);
                        
                        if (parm.startsWith("DCC SEND")) {
                            QStringList parts = parm.split(' ', Qt::SkipEmptyParts);
                            if (parts.size() >= 5) {
                                QString filename = parts[2];
                                QString ipString = parts[3];
                                QString portString = parts[4];
                                bool ok1, ok2;
                                uint ipInt = ipString.toUInt(&ok1);
                                ushort port = portString.toUShort(&ok2);
                                if (ok1 && ok2 && port > 0) {
                                    QByteArray ipBytes = PacketParser::Utils::ByteConverter::ToByteArray(ipInt, false);
                                    quint32 ipAddrReversed = (quint8(ipBytes[3]) << 24) | (quint8(ipBytes[2]) << 16) | (quint8(ipBytes[1]) << 8) | quint8(ipBytes[0]);
                                    QHostAddress ip(ipAddrReversed);
                                }
                            }
                        }
                    }
                    QString message = parameters[1];
                    while(message.startsWith(CTCP_DELIMITER)) message.remove(0, 1);
                    while(message.endsWith(CTCP_DELIMITER)) message.remove(message.length()-1, 1);

                    MainPacketHandler()->OnMessageDetected(Events::MessageEventArgs(
                        ApplicationLayerProtocol::IRC, sourceHost, destinationHost,
                        ircPacket->ParentFrame()->GetFrameNumber(), ircPacket->ParentFrame()->GetTimestamp(),
                        from, parameters[0], message, message, attributes, ircPacket->PacketLength()));
                }
            }
        }
        if (tmpCol.size() > 0) {
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                ircPacket->ParentFrame()->GetFrameNumber(), tcpSession->GetFlow()->FiveTuple(),
                transferIsClientToServer, tmpCol, tcpPacket->ParentFrame()->GetTimestamp(), "IRC packet"));
            return ircPacket->ParsedBytesCount();
        }
    }
    return 0;
}

void IrcPacketHandler::Reset() {
    ircSessionList.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
