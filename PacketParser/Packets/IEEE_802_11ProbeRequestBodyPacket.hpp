#pragma once
#include "AbstractPacket.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

class IEEE_802_11ProbeRequestBodyPacket : public AbstractPacket {
private:
    QString requestedSsid;

public:
    IEEE_802_11ProbeRequestBodyPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    
    QString RequestedSsid() const { return requestedSsid; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
