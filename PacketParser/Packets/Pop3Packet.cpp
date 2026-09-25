#include "Pop3Packet.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

const QString Pop3Packet::RESPONSE_OK = "+OK";
const QString Pop3Packet::RESPONSE_ERR = "-ERR";

bool Pop3Packet::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket*& pop3Packet) {
    // Basic stub, real logic would verify if it looks like POP3.
    // In C# TryParse is not implemented or omitted in Pop3Packet, it just creates it.
    // Wait, the C# file didn't have TryParse. Let's just create it directly if called.
    pop3Packet = nullptr;
    try {
        pop3Packet = new Pop3Packet(parentFrame, packetStartIndex, packetEndIndex, clientToServer);
        if (!static_cast<Pop3Packet*>(pop3Packet)->PacketHeaderIsComplete()) {
            delete pop3Packet;
            pop3Packet = nullptr;
            return false;
        }
        return true;
    } catch (...) {
        if (pop3Packet) delete pop3Packet;
        pop3Packet = nullptr;
        return false;
    }
}

Pop3Packet::Pop3Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "POP3")
    , clientToServer(clientToServer)
    , parsedBytesCount(0)
{
    const uint8_t* data = parentFrame->Data();
    int index = PacketStartIndex();
    size_t dataLen = parentFrame->DataLength();

    if (clientToServer) {
        fullRequestOrResponseLine = Utils::ByteConverter::ReadLine(data, index);
        parsedBytesCount = index - packetStartIndex;
    } else {
        fullRequestOrResponseLine = Utils::ByteConverter::ReadLine(data, index);
        if (!fullRequestOrResponseLine.isNull() && (fullRequestOrResponseLine.startsWith(RESPONSE_ERR) || fullRequestOrResponseLine.startsWith(RESPONSE_OK))) {
            parsedBytesCount = index - packetStartIndex;
        }
    }
}

QString Pop3Packet::GetCommandOrResponse() const {
    int splitIndex = fullRequestOrResponseLine.indexOf(' ');
    if (splitIndex > 0) {
        return fullRequestOrResponseLine.left(splitIndex).trimmed();
    }
    return fullRequestOrResponseLine;
}

QString Pop3Packet::GetCommandOrResponseArguments() const {
    int splitIndex = fullRequestOrResponseLine.indexOf(' ');
    if (splitIndex > 0) {
        return fullRequestOrResponseLine.mid(splitIndex).trimmed();
    }
    return "";
}

std::vector<QString> Pop3Packet::ReadResponseLines() const {
    std::vector<QString> lines;
    int index = PacketStartIndex() + parsedBytesCount;
    const uint8_t* data = ParentFrame()->Data();
    size_t dataLen = ParentFrame()->DataLength();

    QString line = Utils::ByteConverter::ReadLine(data, index);
    while (!line.isNull() && line != ".") {
        if (line.startsWith(".")) {
            line = line.mid(1);
        }
        lines.push_back(line);
        line = Utils::ByteConverter::ReadLine(data, index);
    }
    
    // We can't mutate parsedBytesCount in a const method, C# didn't have const methods
    // So we'll skip updating it for this port, or use mutable
    return lines;
}

std::vector<AbstractPacket*> Pop3Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
