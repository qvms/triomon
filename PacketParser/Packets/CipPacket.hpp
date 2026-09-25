#pragma once
#include "AbstractPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class CipPacket : public AbstractPacket {
public:
    CipPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
        : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "CIP") {}
    virtual ~CipPacket() = default;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override {
        std::vector<AbstractPacket*> subPackets;
        if (includeSelfReference) subPackets.push_back(this);
        return subPackets;
    }
};

} }
