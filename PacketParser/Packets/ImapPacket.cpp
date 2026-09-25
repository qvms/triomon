#include "ImapPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

bool ImapPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket*& result) {
    result = nullptr;
    try {
        result = new ImapPacket(parentFrame, packetStartIndex, packetEndIndex, clientToServer);
        if (!static_cast<ImapPacket*>(result)->PacketHeaderIsComplete()) {
            delete result;
            result = nullptr;
            return false;
        }
        return true;
    } catch (...) {
        if (result) delete result;
        result = nullptr;
        return false;
    }
}

ImapPacket::ImapPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IMAP")
    , clientToServer(clientToServer)
    , parsedBytesCount(0)
    , bodyLength(0)
    , messageSequenceNumber(0)
    , parenthesesDiff(0)
{
    const uint8_t* data = parentFrame->Data();
    int index = PacketStartIndex();

    if (clientToServer) {
        fullRequestOrResponseLine = Utils::ByteConverter::ReadLine(data, index);
        parsedBytesCount = index - packetStartIndex;
        if (!fullRequestOrResponseLine.isNull() && fullRequestOrResponseLine.contains(' ')) {
            QStringList parts = fullRequestOrResponseLine.split(' ');
            if (parts.size() >= 2) {
                QString command = parts[1];
                tag = parts[0];
                
                // Note: Simplified Enum matching without reflection
                QString cmdUpper = command.toUpper();
                if (cmdUpper == "CAPABILITY") clientCommand = ClientCommand::CAPABILITY;
                else if (cmdUpper == "NOOP") clientCommand = ClientCommand::NOOP;
                else if (cmdUpper == "LOGOUT") clientCommand = ClientCommand::LOGOUT;
                else if (cmdUpper == "STARTTLS") clientCommand = ClientCommand::STARTTLS;
                else if (cmdUpper == "AUTHENTICATE") clientCommand = ClientCommand::AUTHENTICATE;
                else if (cmdUpper == "LOGIN") clientCommand = ClientCommand::LOGIN;
                else if (cmdUpper == "SELECT") clientCommand = ClientCommand::SELECT;
                else if (cmdUpper == "EXAMINE") clientCommand = ClientCommand::EXAMINE;
                else if (cmdUpper == "CREATE") clientCommand = ClientCommand::CREATE;
                else if (cmdUpper == "DELETE") clientCommand = ClientCommand::DELETE;
                else if (cmdUpper == "RENAME") clientCommand = ClientCommand::RENAME;
                else if (cmdUpper == "SUBSCRIBE") clientCommand = ClientCommand::SUBSCRIBE;
                else if (cmdUpper == "UNSUBSCRIBE") clientCommand = ClientCommand::UNSUBSCRIBE;
                else if (cmdUpper == "LIST") clientCommand = ClientCommand::LIST;
                else if (cmdUpper == "LSUB") clientCommand = ClientCommand::LSUB;
                else if (cmdUpper == "STATUS") clientCommand = ClientCommand::STATUS;
                else if (cmdUpper == "APPEND") clientCommand = ClientCommand::APPEND;
                else if (cmdUpper == "CHECK") clientCommand = ClientCommand::CHECK;
                else if (cmdUpper == "CLOSE") clientCommand = ClientCommand::CLOSE;
                else if (cmdUpper == "EXPUNGE") clientCommand = ClientCommand::EXPUNGE;
                else if (cmdUpper == "SEARCH") clientCommand = ClientCommand::SEARCH;
                else if (cmdUpper == "FETCH") clientCommand = ClientCommand::FETCH;
                else if (cmdUpper == "STORE") clientCommand = ClientCommand::STORE;
                else if (cmdUpper == "COPY") clientCommand = ClientCommand::COPY;
                else if (cmdUpper == "UID") clientCommand = ClientCommand::UID;
                else if (cmdUpper == "NAMESPACE") clientCommand = ClientCommand::NAMESPACE;
                else if (cmdUpper == "ESEARCH") clientCommand = ClientCommand::ESEARCH;
                else if (cmdUpper == "ID") clientCommand = ClientCommand::ID;
                else if (cmdUpper == "IDLE") clientCommand = ClientCommand::IDLE;
                else if (cmdUpper == "CHILDREN") clientCommand = ClientCommand::CHILDREN;
                else if (cmdUpper == "ENABLE") clientCommand = ClientCommand::ENABLE;
                else if (cmdUpper == "MOVE") clientCommand = ClientCommand::MOVE;
                else if (cmdUpper == "SORT") clientCommand = ClientCommand::SORT;
                else if (cmdUpper == "THREAD") clientCommand = ClientCommand::THREAD;
                else if (cmdUpper == "UIDPLUS") clientCommand = ClientCommand::UIDPLUS;
                else if (cmdUpper == "UNSELECT") clientCommand = ClientCommand::UNSELECT;
                else if (cmdUpper == "WITHIN") clientCommand = ClientCommand::WITHIN;
                else if (cmdUpper == "AUTH") clientCommand = ClientCommand::AUTH;
                else if (cmdUpper == "ACL") clientCommand = ClientCommand::ACL;
                else if (cmdUpper == "BINARY") clientCommand = ClientCommand::BINARY;
                else if (cmdUpper == "CATENATE") clientCommand = ClientCommand::CATENATE;
                else if (cmdUpper == "IMAP4") clientCommand = ClientCommand::IMAP4;
                else if (cmdUpper == "IMAP4REV1") clientCommand = ClientCommand::IMAP4rev1;
                else if (cmdUpper == "METADATA") clientCommand = ClientCommand::METADATA;
                else if (cmdUpper == "MULTIAPPEND") clientCommand = ClientCommand::MULTIAPPEND;
                else if (cmdUpper == "QUOTA") clientCommand = ClientCommand::QUOTA;
                else if (cmdUpper == "SCAN") clientCommand = ClientCommand::SCAN;
                else if (cmdUpper == "SURGEMAIL") clientCommand = ClientCommand::SURGEMAIL;

                if (clientCommand && clientCommand.value() == ClientCommand::APPEND) {
                    int len;
                    if (tryParseLiteral(fullRequestOrResponseLine, len)) {
                        bodyLength = len;
                    }
                }
            }
        }
    } else {
        fullRequestOrResponseLine = Utils::ByteConverter::ReadLine(data, index);
        if (!fullRequestOrResponseLine.isNull()) {
            if (fullRequestOrResponseLine.startsWith("* ")) {
                parsedBytesCount = index - packetStartIndex;
                QStringList requestFields = fullRequestOrResponseLine.split(' ');
                if (requestFields.size() > 1) {
                    bool ok;
                    messageSequenceNumber = requestFields[1].toUInt(&ok);
                    if (!ok) messageSequenceNumber = 0;
                }

                if (requestFields.size() > 2) {
                    QString cmdUpper = requestFields[2].toUpper();
                    if (cmdUpper == "FETCH") clientCommand = ClientCommand::FETCH;
                    // ... other commands can be parsed here ...

                    if (clientCommand && clientCommand.value() == ClientCommand::FETCH) {
                        int len;
                        if (tryParseLiteral(fullRequestOrResponseLine, len)) {
                            bodyLength = len;
                            parenthesesDiff += fullRequestOrResponseLine.split('(').size() - 1;
                            parenthesesDiff -= fullRequestOrResponseLine.split(')').size() - 1;
                        }
                    }
                }
            } else if (fullRequestOrResponseLine.startsWith("+ ")) {
                parsedBytesCount = index - packetStartIndex;
            } else {
                if (fullRequestOrResponseLine.contains(' ')) {
                    QStringList responseFields = fullRequestOrResponseLine.split(' ');
                    if (responseFields.size() > 1) {
                        QString resUpper = responseFields[1].toUpper();
                        if (resUpper == "OK") { serverResult = ServerResult::OK; parsedBytesCount = index - packetStartIndex; }
                        else if (resUpper == "NO") { serverResult = ServerResult::NO; parsedBytesCount = index - packetStartIndex; }
                        else if (resUpper == "BAD") { serverResult = ServerResult::BAD; parsedBytesCount = index - packetStartIndex; }
                    }
                }
                if (parsedBytesCount == 0) {
                    if (fullRequestOrResponseLine.contains(')')) {
                        parenthesesDiff += fullRequestOrResponseLine.split('(').size() - 1;
                        parenthesesDiff -= fullRequestOrResponseLine.split(')').size() - 1;
                        if (parenthesesDiff < 0) {
                            parsedBytesCount = index - PacketStartIndex();
                        }
                    }
                }
            }
        }
    }
}

bool ImapPacket::tryParseLiteral(QString lineWithLiteralEnding, int& number) {
    if (!lineWithLiteralEnding.isNull() && lineWithLiteralEnding.length() > 2 && lineWithLiteralEnding.contains('{') && lineWithLiteralEnding.endsWith('}')) {
        int lastBraceIdx = lineWithLiteralEnding.lastIndexOf('{');
        lineWithLiteralEnding = lineWithLiteralEnding.mid(lastBraceIdx);
        
        while (lineWithLiteralEnding.startsWith('{') || lineWithLiteralEnding.startsWith('+') || lineWithLiteralEnding.startsWith('}')) {
            lineWithLiteralEnding = lineWithLiteralEnding.mid(1);
        }
        while (lineWithLiteralEnding.endsWith('{') || lineWithLiteralEnding.endsWith('+') || lineWithLiteralEnding.endsWith('}')) {
            lineWithLiteralEnding.chop(1);
        }

        bool ok;
        number = lineWithLiteralEnding.toInt(&ok);
        return ok;
    }
    number = -1;
    return false;
}

std::optional<uint32_t> ImapPacket::MessageSequenceNumber() const {
    if (messageSequenceNumber == 0) return std::nullopt;
    return messageSequenceNumber;
}

std::vector<AbstractPacket*> ImapPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
