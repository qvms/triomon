#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include "../Utils.hpp"
#include <QString>
#include <QDomDocument>

namespace PacketParser {
namespace Packets {

class RmsPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    uint64_t NextSegmentLength;
    bool HasNextSegmentLength;
    QDomDocument Payload;
    bool m_packetHeaderIsComplete;
    int m_parsedBytesCount;

    RmsPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override { return m_packetHeaderIsComplete; }
    int ParsedBytesCount() const override { return m_parsedBytesCount; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
