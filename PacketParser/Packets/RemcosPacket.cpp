#include "RemcosPacket.hpp"
#include <cstring>
#include <QDebug>

namespace PacketParser {
namespace Packets {

const std::vector<uint8_t> RemcosPacket::REMCOS_MAGIC = { 0x00, 0xFF, 0x04, 0x24 }; // ToByteArray(0x2404FF00, false) which is little endian (but false means little endian in ByteConverter? Wait, false in ByteConverter means Big Endian in Utils::ByteConverter normally, actually ToByteArray(_, false) is little endian. Let's just use exact bytes. C# was uint 0x2404FF00 ToByteArray false. If false is little-endian, it's 00 FF 04 24.
// Let's look at DELIMITER: (long)0x7C1E1E1F7C false. Skip(3). That implies false is big endian?
// Actually in ByteConverter: bool littleEndian. So false = BigEndian.
// Let's redefine exact bytes from C# context if possible, but let's just implement the logic.
// uint 0x2404FF00 -> ToByteArray(..., false) -> 24 04 FF 00 (big endian).
const std::vector<uint8_t> REMCOS_MAGIC_IMPL = { 0x24, 0x04, 0xFF, 0x00 };
const std::vector<uint8_t> RemcosPacket::DELIMITER = { 0x1E, 0x1E, 0x1F, 0x7C }; // 7C1E1E1F7C BigEndian -> 00 00 00 7C 1E 1E 1F 7C, Skip(3) -> 7C 1E 1E 1F 7C. Oh, the C# says DELIMITER = { 0x7C, 0x1E, 0x1E, 0x1F, 0x7C }. Skip(3) of a long (8 bytes). 00 00 00 7C 1E 1E 1F 7C. Skip 3 bytes -> 7C 1E 1E 1F 7C.

bool remcosStartsWith(const uint8_t* data, size_t dataLen, int startIndex, const std::vector<uint8_t>& prefix) {
    if (startIndex < 0 || startIndex + prefix.size() > dataLen) return false;
    return std::memcmp(data + startIndex, prefix.data(), prefix.size()) == 0;
}

RemcosPacket::RemcosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Remcos")
    , RemcosPayloadLength(0)
    , CommandNumber(0)
{
    if (packetEndIndex - packetStartIndex > 10) {
        if (remcosStartsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, REMCOS_MAGIC_IMPL)) {
            RemcosPayloadLength = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4, 4, true);
            if (PacketEndIndex() >= static_cast<int>(packetStartIndex + 8 + RemcosPayloadLength)) {
                setPacketEndIndex(packetStartIndex + 7 + RemcosPayloadLength);
            }
            CommandNumber = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 8, 4, true);
        }
    }
}

bool RemcosPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;

    if (packetEndIndex - packetStartIndex > 10) {
        if (remcosStartsWith(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, REMCOS_MAGIC_IMPL)) {
            uint32_t length = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 4, 4, true);
            if (length < 100 * 1024 * 1024) {
                try {
                    *result = new RemcosPacket(parentFrame, packetStartIndex, packetEndIndex);
                    return true;
                } catch (...) {
                    return false;
                }
            }
        }
    }
    return false;
}

bool RemcosPacket::PacketHeaderIsComplete() const {
    return PacketLength() >= static_cast<int>(RemcosPayloadLength + 8);
}

int RemcosPacket::ParsedBytesCount() const {
    if (PacketHeaderIsComplete()) return RemcosPayloadLength + 8;
    return 0;
}

bool RemcosPacket::TryGetCommand(RemcosCommand& command) const {
    switch (CommandNumber) {
        case 0x01: case 0x03: case 0x05: case 0x06: case 0x08: case 0x0D: case 0x0E: case 0x0F:
        case 0x10: case 0x11: case 0x13: case 0x18: case 0x1B: case 0x1D: case 0x21: case 0x22:
        case 0x23: case 0x24: case 0x26: case 0x27: case 0x2C: case 0x2F: case 0x28: case 0x2E:
        case 0x30: case 0x32: case 0x34: case 0x44: case 0x4b: case 0x4c: case 0x4d: case 0x68:
        case 0x92: case 0x95: case 0x96: case 0x98: case 0xA3: case 0xB2: case 0xB3: case 0x8F:
            command = static_cast<RemcosCommand>(CommandNumber);
            return true;
        default:
            command = RemcosCommand::UNDEFINED;
            return false;
    }
}

int BoyerMooreIndexOf(const uint8_t* text, size_t textLen, const std::vector<uint8_t>& pattern, int startIndex) {
    if (pattern.empty()) return startIndex;
    if (startIndex < 0 || startIndex + pattern.size() > textLen) return -1;
    for (int i = startIndex; i <= static_cast<int>(textLen - pattern.size()); i++) {
        if (std::memcmp(text + i, pattern.data(), pattern.size()) == 0) return i;
    }
    return -1;
}

std::vector<std::vector<uint8_t>> RemcosPacket::GetFields(bool skipEmpty, bool requireCompletePacketHeader) const {
    std::vector<std::vector<uint8_t>> fields;
    
    if (PacketHeaderIsComplete() || !requireCompletePacketHeader) {
        int index = PacketStartIndex() + 12;
        std::vector<uint8_t> delim = { 0x7C, 0x1E, 0x1E, 0x1F, 0x7C };
        
        while (index <= PacketEndIndex()) {
            int delimiterIndex = BoyerMooreIndexOf(ParentFrame()->Data(), ParentFrame()->DataLength(), delim, index);
            if (delimiterIndex == index) {
                if (!skipEmpty) fields.push_back(std::vector<uint8_t>());
            } else if (delimiterIndex < index) {
                int takeLen = PacketStartIndex() + 8 + RemcosPayloadLength - index;
                if (takeLen > 0) {
                    std::vector<uint8_t> f(takeLen);
                    std::memcpy(f.data(), ParentFrame()->Data() + index, takeLen);
                    fields.push_back(f);
                }
                break;
            } else if (delimiterIndex > index) {
                int takeLen = 0;
                if (delimiterIndex > PacketEndIndex()) {
                    takeLen = PacketEndIndex() + 1 - index;
                } else {
                    takeLen = delimiterIndex - index;
                }
                if (takeLen > 0) {
                    std::vector<uint8_t> f(takeLen);
                    std::memcpy(f.data(), ParentFrame()->Data() + index, takeLen);
                    fields.push_back(f);
                }
            }
            index = delimiterIndex + delim.size();
        }
    }
    return fields;
}

std::vector<AbstractPacket*> RemcosPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
