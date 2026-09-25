#include "NetBiosPacket.hpp"
#include "NetBiosNameServicePacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

NetBiosPacket::NetBiosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const QString& packetTypeDescription)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, packetTypeDescription)
{
}

QString NetBiosPacket::DecodeNetBiosName(const Frame* parentFrame, int& frameIndex, NetBiosNameServicePacket* nbnsPacket) {
    int initialFrameIndex = frameIndex;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (frameIndex >= static_cast<int>(dataLen)) return QString();

    uint8_t labelByteCount = data[frameIndex];
    if (nbnsPacket != nullptr) {
        if (labelByteCount >= 0xc0) {
            int pointerIndex = nbnsPacket->PacketStartIndex() + (Utils::ByteConverter::ToUInt16(data, frameIndex) & 0x3fff);
            frameIndex += 2;
            return DecodeNetBiosName(parentFrame, pointerIndex, nullptr); // nbnsPacket passed as nullptr for pointers generally unless we want full recursive
        } else {
            labelByteCount = labelByteCount & 0x3f;
        }
    } else {
        if (!parentFrame->QuickParse() && labelByteCount > 63) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, frameIndex, frameIndex, QString("NetBios Name label is larger than 63 : %1").arg(labelByteCount)));
        }
    }
    frameIndex++;

    QString decodedName = GetNetBiosNameFromNibbles(data, frameIndex, labelByteCount, initialFrameIndex);

    if (decodedName.length() > 0 && (decodedName.at(0).unicode() > 0x100 || decodedName.at(0).category() == QChar::Other_Control)) {
        QString sb;
        if (TryParseNetBiosName(data, frameIndex - labelByteCount, labelByteCount * 2, sb)) {
            decodedName = sb;
        }
    }

    while (frameIndex < static_cast<int>(dataLen) && data[frameIndex] != 0x00 && frameIndex < initialFrameIndex + 255) {
        decodedName.append(".");
        labelByteCount = data[frameIndex];
        if (!parentFrame->QuickParse() && labelByteCount > 63) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, frameIndex, frameIndex, QString("NetBios Name label is larger than 63 : %1").arg(labelByteCount)));
        }
        frameIndex++;
        for (uint8_t b = 0; b < labelByteCount; b++) {
            if (frameIndex < static_cast<int>(dataLen)) {
                decodedName.append(static_cast<char>(data[frameIndex]));
                frameIndex++;
            }
        }
    }
    frameIndex++;
    return decodedName;
}

QString NetBiosPacket::GetNetBiosNameFromNibbles(const uint8_t* data, int& index, int nibbleCount, int initialFrameIndex) {
    QString decodedName = "";
    for (uint8_t b = 0; b < nibbleCount; b += 2) {
        uint8_t b1 = data[index];
        uint8_t b2 = data[index + 1];
        char c = static_cast<char>(((b1 - 0x41) << 4) + (b2 - 0x41));
        if (b == nibbleCount - 2 && index == initialFrameIndex + 1 + 2 * 15) {
            if (decodedName.length() != 1 || decodedName.at(0) != '*') {
                decodedName.append(QString("<%1>").arg(static_cast<uint8_t>(c), 2, 16, QChar('0')).toUpper());
            }
        } else if (c != static_cast<char>(0x20) && c != static_cast<char>(0x00)) {
            decodedName.append(c);
        }
        index += 2;
    }
    return decodedName;
}

bool NetBiosPacket::TryParseNetBiosName(const uint8_t* data, int startIndex, int length, QString& sb) {
    int idx = startIndex;
    QString netBiosRaw = Utils::ByteConverter::ReadNullTerminatedString(data, idx, false, false, length);
    netBiosRaw = netBiosRaw.trimmed();
    std::vector<uint8_t> nibbles;
    for (int i = 0; i < netBiosRaw.length(); ++i) {
        QChar c = netBiosRaw.at(i);
        if (c.unicode() < 0x41 || c.unicode() > 0x140) {
            return false;
        }
        nibbles.push_back(static_cast<uint8_t>(c.unicode()));
    }
    if (nibbles.size() < 2) {
        return false;
    }
    int index = 0;
    try {
        sb = GetNetBiosNameFromNibbles(nibbles.data(), index, nibbles.size() - (nibbles.size() % 2), 0);
        return sb.length() > 0 && sb.at(0).unicode() < 0x100 && sb.at(0).category() != QChar::Other_Control;
    } catch (...) {
        return false;
    }
}

} // namespace Packets
} // namespace PacketParser
