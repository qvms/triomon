#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class Pop3Packet : public AbstractPacket {
public:
    enum class ClientCommand {
        APOP, DELE, LIST, LIST_WITH_ARGS, NOOP, PASS, QUIT, RETR, RSET, STAT, TOP, UIDL, UIDL_WITH_ARGS, USER, CAPA, AUTH, AUTH_PLAIN
    };

    static const QString RESPONSE_OK;
    static const QString RESPONSE_ERR;

private:
    bool clientToServer;
    QString fullRequestOrResponseLine;
    int parsedBytesCount;

    Pop3Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket*& pop3Packet);

    QString FullRequestOrResponseLine() const { return fullRequestOrResponseLine; }
    bool ClientToServer() const { return clientToServer; }
    bool PacketHeaderIsComplete() const { return parsedBytesCount > 0; }
    int ParsedBytesCount() const { return parsedBytesCount; }

    QString GetCommandOrResponse() const;
    QString GetCommandOrResponseArguments() const;
    std::vector<QString> ReadResponseLines() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
