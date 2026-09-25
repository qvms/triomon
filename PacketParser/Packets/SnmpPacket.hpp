#pragma once
#include "AbstractPacket.hpp"
#include "../Utils.hpp"
#include <QString>
#include <QStringList>

namespace PacketParser {
namespace Packets {

class SnmpPacket : public AbstractPacket {
public:
    enum class Version : uint8_t { SNMPv1 = 0, SNMPv2c = 1, SNMPv3 = 3 };

    uint8_t VersionRaw;
    QString CommunityString;
    QStringList CarvedStrings;

    SnmpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
