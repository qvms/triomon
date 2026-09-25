#include "SyslogPacket.hpp"

namespace PacketParser {
namespace Packets {

SyslogPacket::SyslogPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Syslog")
{
    if (packetEndIndex >= packetStartIndex) {
        SyslogMessage = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, packetEndIndex - packetStartIndex + 1, false);
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Message", SyslogMessage);
        }
    }
}

std::vector<AbstractPacket*> SyslogPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
