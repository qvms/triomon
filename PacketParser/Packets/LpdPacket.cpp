#include "LpdPacket.hpp"
#include "../Utils.hpp"
#include <QRegularExpression>
#include <cctype>
#include <algorithm>

namespace PacketParser {
namespace Packets {

const std::vector<char> WHITE_SPACE_CHARS = { ' ', '\t', 0x0b, 0x0c };

bool LpdPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket** result) {
    *result = nullptr;
    try {
        if (clientToServer) {
            uint8_t cmdCode;
            QStringList operands;
            int bytesRead;
            if (LpdRequestPacket::TryParseCommandLine(parentFrame->Data(), packetStartIndex, packetEndIndex, cmdCode, operands, bytesRead)) {
                *result = new LpdRequestPacket(parentFrame, packetStartIndex, packetEndIndex);
            } else {
                return LpdControlFilePacket::TryParse(parentFrame, packetStartIndex, packetEndIndex, result);
            }
        } else if (packetStartIndex == packetEndIndex) {
            *result = new LpdResponsePacket(parentFrame, packetStartIndex);
        }
    } catch (...) {
    }
    return *result != nullptr;
}

bool LpdPacket::PacketHeaderIsComplete() const {
    return true;
}

LpdPacket::LpdPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "LPD") {
}

std::vector<AbstractPacket*> LpdPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}


bool LpdPacket::LpdRequestPacket::TryParseCommandLine(const uint8_t* data, int startIndex, int endIndex, uint8_t& commandCode, QStringList& operands, int& bytesRead) {
    commandCode = data[startIndex];
    if (commandCode < 1 || commandCode > 5) {
        bytesRead = 0;
        operands.clear();
        return false;
    }
    int index = startIndex + 1;
    try {
        QString line = Utils::ByteConverter::ReadLine(data, index, endIndex - index + 1);
        bytesRead = index - startIndex;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        operands = line.split(QRegExp("[ \t\x0B\x0C]"), QString::SkipEmptyParts);
#else
        operands = line.split(QRegularExpression("[ \t\x0B\x0C]"), Qt::SkipEmptyParts);
#endif
        return index <= endIndex + 1 && data[index - 1] == 0x0a;
    } catch (...) {
        bytesRead = 0;
        operands.clear();
        return false;
    }
}

LpdPacket::LpdRequestPacket::LpdRequestPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : LpdPacket(parentFrame, packetStartIndex, packetEndIndex) {
    if (!TryParseCommandLine(parentFrame->Data(), packetStartIndex, packetEndIndex, CommandCode, Operands, m_parsedBytesCount)) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Invalid LPD client command"));
    } else if (!parentFrame->QuickParse()) {
        DaemonCommandCode cmdCode;
        if (TryGetCommand(cmdCode)) {
            // Can add specific attributes here if needed
        }
    }
}

int LpdPacket::LpdRequestPacket::ParsedBytesCount() const {
    return m_parsedBytesCount;
}

bool LpdPacket::LpdRequestPacket::TryGetCommand(DaemonCommandCode& command) const {
    if (CommandCode >= 1 && CommandCode <= 5) {
        command = static_cast<DaemonCommandCode>(CommandCode);
        return true;
    } else {
        command = DaemonCommandCode::PrintWaitingJobs;
        return false;
    }
}

bool LpdPacket::LpdRequestPacket::TryGetSubCommand(ReceiveJobSubcommandCode& subCommand) const {
    if (CommandCode >= 1 && CommandCode <= 3) {
        subCommand = static_cast<ReceiveJobSubcommandCode>(CommandCode);
        return true;
    } else {
        subCommand = ReceiveJobSubcommandCode::AbortJob;
        return false;
    }
}

LpdPacket::LpdResponsePacket::LpdResponsePacket(const Frame* parentFrame, int packetStartIndex)
    : LpdPacket(parentFrame, packetStartIndex, packetStartIndex) {
    ResponseCode = parentFrame->Data()[packetStartIndex];
    if (!parentFrame->QuickParse()) {
        addAttribute("ResponseCode", QString::number(ResponseCode));
    }
}

int LpdPacket::LpdResponsePacket::ParsedBytesCount() const {
    return 1;
}

bool LpdPacket::LpdControlFilePacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    *result = nullptr;
    if (parentFrame->Data()[packetEndIndex] != 0x00) return false;
    if (packetEndIndex > packetStartIndex && parentFrame->Data()[packetEndIndex - 1] != 0x0a) return false;
    for (int i = packetStartIndex; i < packetEndIndex - 1; i++) {
        uint8_t ch = parentFrame->Data()[i];
        if (ch > 126) return false;
        bool isWhiteSpace = std::find(WHITE_SPACE_CHARS.begin(), WHITE_SPACE_CHARS.end(), static_cast<char>(ch)) != WHITE_SPACE_CHARS.end();
        if (ch != 0x0a && std::iscntrl(ch) && !isWhiteSpace) return false;
    }
    try {
        *result = new LpdControlFilePacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

LpdPacket::LpdControlFilePacket::LpdControlFilePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : LpdPacket(parentFrame, packetStartIndex, packetEndIndex) {
    int index = packetStartIndex;
    while (index < packetEndIndex - 1) {
        QString line = Utils::ByteConverter::ReadLine(parentFrame->Data(), index, packetEndIndex - index);
        if (line.isEmpty()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Invalid LPD Control File Command Line"));
            return;
        }
        Command command = static_cast<Command>(line[0].toLatin1());
        CommandList.push_back({ command, line.mid(1) });
    }
}

int LpdPacket::LpdControlFilePacket::ParsedBytesCount() const {
    return PacketEndIndex() - PacketStartIndex() + 1;
}

} }
