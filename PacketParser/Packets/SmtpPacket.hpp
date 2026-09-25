#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <utility>

namespace PacketParser {
namespace Packets {

class SmtpPacket : public AbstractPacket {
public:
    enum class ClientCommands {
        HELO, MAIL, RCPT, DATA, RSET, SEND, SOML, SAML, VRFY, EXPN, HELP, NOOP, QUIT, TURN, EHLO, AUTH, STARTTLS
    };

private:
    bool clientToServer;
    std::vector<std::pair<QString, QString>> requestCommandAndArgumentList;
    std::vector<std::pair<int, QString>> replyList;

    SmtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort, AbstractPacket*& smtpPacket);

    bool ClientToServer() const { return clientToServer; }
    const std::vector<std::pair<QString, QString>>& RequestCommandsAndArguments() const { return requestCommandAndArgumentList; }
    const std::vector<std::pair<int, QString>>& Replies() const { return replyList; }
    
    std::vector<int> ReplyCodes() const;
    QString ReadLine() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
