#include "IrcPacket.hpp"
#include "../Utils.hpp"
#include <QStringBuilder>

namespace PacketParser {
namespace Packets {

IrcPacket::Message::Message(const QByteArray& prefix, const QByteArray& command, const std::vector<QByteArray>& parameters)
    : prefix(prefix), command(command), parameters(parameters) {}

QString IrcPacket::Message::Prefix() const {
    if (prefix.isEmpty()) return QString();
    return QString::fromUtf8(prefix);
}

QString IrcPacket::Message::Command() const {
    if (command.isEmpty()) return QString();
    return QString::fromUtf8(command);
}

std::vector<QString> IrcPacket::Message::Parameters() const {
    std::vector<QString> params;
    for (const auto& p : parameters) {
        params.push_back(QString::fromUtf8(p));
    }
    return params;
}

QString IrcPacket::Message::ToString() const {
    QString returnString;
    if (!prefix.isEmpty()) {
        returnString += ":" + Prefix();
    }
    if (!returnString.isEmpty()) returnString += " ";
    returnString += Command();
    for (const QString& p : Parameters()) {
        returnString += " " + p;
    }
    return returnString.trimmed();
}

bool IrcPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result) {
    result = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex >= static_cast<int>(dataLen)) return false;

    char firstChar = static_cast<char>(data[packetStartIndex]);
    if (!QChar(firstChar).isDigit() && !QChar(firstChar).isLetter() && firstChar != ':') {
        return false;
    }

    int lfIndex = -1;
    for (int i = packetStartIndex; i <= packetEndIndex && i < static_cast<int>(dataLen); i++) {
        if (data[i] == static_cast<uint8_t>(IrcChars::LF)) {
            lfIndex = i;
            break;
        }
    }

    if (lfIndex == -1) {
        if (packetEndIndex - packetStartIndex > MAX_MESSAGE_LENGTH) return false;
    }

    int crIndex = -1;
    for (int i = packetStartIndex; i <= packetEndIndex && i < static_cast<int>(dataLen); i++) {
        if (data[i] == static_cast<uint8_t>(IrcChars::CR)) {
            crIndex = i;
            break;
        }
    }

    if (crIndex >= packetEndIndex || (crIndex != -1 && crIndex + 1 < static_cast<int>(dataLen) && data[crIndex + 1] != static_cast<uint8_t>(IrcChars::LF))) {
        if (packetEndIndex - packetStartIndex > MAX_MESSAGE_LENGTH) return false;
    }

    try {
        result = new IrcPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        if (result) delete result;
        result = nullptr;
        return false;
    }
}

IrcPacket::IrcPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IRC")
    , packetHeaderIsComplete(false)
    , parsedBytesCount(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();
    int index = packetStartIndex;

    while (index < packetEndIndex && index < static_cast<int>(dataLen)) {
        QByteArray prefix;
        if (data[index] == static_cast<uint8_t>(IrcChars::Colon)) {
            index++;
            prefix = Utils::ByteConverter::ToByteArray(data, index, static_cast<uint8_t>(IrcChars::Space), false);
        }

        while (index < static_cast<int>(dataLen) && data[index] == static_cast<uint8_t>(IrcChars::Space)) {
            index++;
        }

        QByteArray command = Utils::ByteConverter::ToByteArray(data, index, static_cast<uint8_t>(IrcChars::Space), false);

        while (index < static_cast<int>(dataLen) && data[index] == static_cast<uint8_t>(IrcChars::Space)) {
            index++;
        }

        std::vector<uint8_t> lineBreakers = { static_cast<uint8_t>(IrcChars::CR), static_cast<uint8_t>(IrcChars::LF) };
        QByteArray allParameterData = Utils::ByteConverter::ToByteArray(data, dataLen, index, lineBreakers, false);

        if (index < static_cast<int>(dataLen)) {
            if (data[index] == static_cast<uint8_t>(IrcChars::LF)) {
                index++;
            }
            packetHeaderIsComplete = true;
            parsedBytesCount = index - packetStartIndex;
        } else if (parsedBytesCount > 0) {
            break;
        }

        std::vector<QByteArray> parameters;
        int pi = 0;
        while (pi < allParameterData.length()) {
            if (static_cast<uint8_t>(allParameterData[pi]) == static_cast<uint8_t>(IrcChars::Colon)) {
                QByteArray trailing = allParameterData.mid(pi + 1);
                parameters.push_back(trailing);
                pi = allParameterData.length();
            } else {
                parameters.push_back(Utils::ByteConverter::ToByteArray(allParameterData, pi, static_cast<uint8_t>(IrcChars::Space), false));
                while (pi < allParameterData.length() && static_cast<uint8_t>(allParameterData[pi]) == static_cast<uint8_t>(IrcChars::Space)) {
                    pi++;
                }
            }
        }
        messages.push_back(Message(prefix, command, parameters));
    }
}

std::vector<AbstractPacket*> IrcPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
