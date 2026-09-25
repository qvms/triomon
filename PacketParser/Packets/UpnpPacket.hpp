#pragma once
#include "AbstractPacket.hpp"
#include "../Utils.hpp"
#include <QString>
#include <QStringList>

namespace PacketParser {
namespace Packets {

class UpnpPacket : public AbstractPacket {
public:
    QStringList FieldList;

    UpnpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
