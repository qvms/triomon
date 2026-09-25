#pragma once
#include "AbstractPacket.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

class NetBiosNameServicePacket;

class NetBiosPacket : public AbstractPacket {
protected:
    NetBiosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const QString& packetTypeDescription);

public:
    static QString DecodeNetBiosName(const Frame* parentFrame, int& frameIndex, NetBiosNameServicePacket* nbnsPacket = nullptr);

private:
    static QString GetNetBiosNameFromNibbles(const uint8_t* data, int& index, int nibbleCount, int initialFrameIndex);
    static bool TryParseNetBiosName(const uint8_t* data, int startIndex, int length, QString& sb);
};

} // namespace Packets
} // namespace PacketParser
