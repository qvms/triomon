#pragma once
#include "AbstractPacket.hpp"
#include <vector>
#include <optional>

namespace PacketParser {
namespace Packets {

class Erspan : public AbstractPacket {
private:
    int erspanHeaderLength;
    uint8_t m_version;
    std::optional<uint16_t> m_vlanID;

public:
    Erspan(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint8_t Version() const { return m_version; }
    std::optional<uint16_t> VlanID() const { return m_vlanID; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
