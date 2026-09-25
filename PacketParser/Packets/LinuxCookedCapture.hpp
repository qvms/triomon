#pragma once
#include "AbstractPacket.hpp"
#include <vector>
#include <QString>

namespace PacketParser {
namespace Packets {

class LinuxCookedCapture : public AbstractPacket {
private:
    static const int SLL_HEADER_LENGTH = 16;
    
    enum class PacketTypes : uint16_t { 
        LINUX_SLL_HOST = 0, 
        LINUX_SLL_BROADCAST = 1, 
        LINUX_SLL_MULTICAST = 2, 
        LINUX_SLL_OTHERHOST = 3, 
        LINUX_SLL_OUTGOING = 4
    };

    uint16_t packetType;
    uint16_t addressType;
    uint16_t addressLength;
    QString sourceMacAddress;
    uint16_t protocol;

public:
    LinuxCookedCapture(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
