#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class LinuxCookedCapture2 : public AbstractPacket {
private:
    static const int SLL2_HEADER_LENGTH = 20;

    enum class PacketTypes : uint16_t { 
        LINUX_SLL_HOST = 0, 
        LINUX_SLL_BROADCAST = 1, 
        LINUX_SLL_MULTICAST = 2, 
        LINUX_SLL_OTHERHOST = 3, 
        LINUX_SLL_OUTGOING = 4
    };

    uint16_t protocol;

public:
    LinuxCookedCapture2(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
