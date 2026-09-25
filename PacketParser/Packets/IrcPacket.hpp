#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QString>
#include <vector>
#include <QByteArray>

namespace PacketParser {
namespace Packets {

class IrcPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    static const int MAX_MESSAGE_LENGTH = 510;

    enum class IrcChars : uint8_t {
        Space = 0x20,
        Colon = 0x3a,
        Nul = 0x00,
        CR = 0x0d,
        LF = 0x0a
    };

    class Message {
    private:
        QByteArray prefix;
        QByteArray command;
        std::vector<QByteArray> parameters;

    public:
        Message(const QByteArray& prefix, const QByteArray& command, const std::vector<QByteArray>& parameters);

        QString Prefix() const;
        QString Command() const;
        std::vector<QString> Parameters() const;
        QString ToString() const;
    };

private:
    std::vector<Message> messages;
    bool packetHeaderIsComplete;
    int parsedBytesCount;

    IrcPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result);

    const std::vector<Message>& Messages() const { return messages; }
    bool PacketHeaderIsComplete() const override { return packetHeaderIsComplete; }
    int ParsedBytesCount() const override { return parsedBytesCount; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
