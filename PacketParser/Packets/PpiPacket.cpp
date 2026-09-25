#include "PpiPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

#if __has_include("PacketFactory.hpp")
#include "PacketFactory.hpp"
#endif

namespace PacketParser {
namespace Packets {

PpiPacket::PpiPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "PPI")
    , ppiLength(0)
    , dataLinkType(static_cast<DataLinkTypeEnum>(0))
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 7 >= static_cast<int>(dataLen)) {
        return; // truncated
    }

    ppiLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, true);
    if (!parentFrame->QuickParse()) {
        addAttribute("Length", QString::number(ppiLength));
    }

    uint32_t dataLinkTypeUInt = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4, true);
    dataLinkType = static_cast<DataLinkTypeEnum>(dataLinkTypeUInt);
    
    if (!parentFrame->QuickParse()) {
        addAttribute("Data Link Type", QString::number(dataLinkTypeUInt));
    }
}

std::vector<AbstractPacket*> PpiPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + ppiLength <= PacketEndIndex()) {
        AbstractPacket* packet = nullptr;
#ifdef __has_include
#if __has_include("PacketFactory.hpp")
        if (!PacketFactory::TryGetPacket(packet, dataLinkType, ParentFrame(), PacketStartIndex() + ppiLength, PacketEndIndex())) {
            packet = nullptr;
        }
#endif
#endif
        if (!packet) {
            packet = new RawPacket(ParentFrame(), PacketStartIndex() + ppiLength, PacketEndIndex());
        }
        
        if (packet) {
            subPackets.push_back(packet);
            std::vector<AbstractPacket*> childSubPackets = packet->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
