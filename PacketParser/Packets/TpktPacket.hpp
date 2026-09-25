#pragma once
#include "AbstractPacket.hpp"
#include "TcpPacket.hpp"
#include "ISessionPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class TpktPacket : public AbstractPacket, public ISessionPacket {
private:
    uint8_t m_version;
    uint16_t m_length;
    const TcpPacket* m_parentTcpPacket;

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const TcpPacket* parentTcpPacket, AbstractPacket*& result);

    TpktPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const TcpPacket* parentTcpPacket);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    bool PacketHeaderIsComplete() const override { return PacketLength() >= 4; }
    int ParsedBytesCount() const override { return PacketLength() >= m_length ? m_length : 0; }
};

} // namespace Packets
} // namespace PacketParser
