#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <optional>

namespace PacketParser {
namespace Packets {

class ImapPacket : public AbstractPacket {
public:
    enum class ClientCommand {
        CAPABILITY, NOOP, LOGOUT, STARTTLS, AUTHENTICATE, LOGIN, SELECT, EXAMINE, CREATE, DELETE, RENAME,
        SUBSCRIBE, UNSUBSCRIBE, LIST, LSUB, STATUS, APPEND, CHECK, CLOSE, EXPUNGE, SEARCH, FETCH, STORE,
        COPY, UID, NAMESPACE, ESEARCH, ID, IDLE, CHILDREN, ENABLE, MOVE, SORT, THREAD, UIDPLUS, UNSELECT,
        WITHIN, AUTH, ACL, BINARY, CATENATE, IMAP4, IMAP4rev1, METADATA, MULTIAPPEND, QUOTA, SCAN, SURGEMAIL
    };

    enum class ServerResult {
        OK, NO, BAD
    };

private:
    bool clientToServer;
    QString fullRequestOrResponseLine;
    int parsedBytesCount;
    std::optional<ClientCommand> clientCommand;
    std::optional<ServerResult> serverResult;
    int bodyLength;
    uint32_t messageSequenceNumber;
    int parenthesesDiff;
    QString tag;

    ImapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
    bool tryParseLiteral(QString lineWithLiteralEnding, int& number);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket*& result);

    QString FullRequestOrResponseLine() const { return fullRequestOrResponseLine; }
    bool ClientToServer() const { return clientToServer; }
    bool PacketHeaderIsComplete() const { return parsedBytesCount > 0; }
    int ParsedBytesCount() const { return parsedBytesCount; }
    void setParsedBytesCount(int value) { parsedBytesCount = value; }
    QString Tag() const { return tag; }
    std::optional<uint32_t> MessageSequenceNumber() const;
    std::optional<ClientCommand> Command() const { return clientCommand; }
    std::optional<ServerResult> Result() const { return serverResult; }
    int BodyLength() const { return bodyLength; }
    int ParenthesesDiff() const { return parenthesesDiff; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
