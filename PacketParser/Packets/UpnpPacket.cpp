#include "UpnpPacket.hpp"

namespace PacketParser {
namespace Packets {

UpnpPacket::UpnpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "UPnP")
{
    int dataIndex = packetStartIndex;
    QString line;
    while (dataIndex < packetEndIndex) {
        line = Utils::ByteConverter::ReadLine(parentFrame->Data(), parentFrame->DataLength(), dataIndex);
        if (!line.isNull() && line.length() > 0) {
            FieldList.append(line);
        } else {
            break;
        }
    }
}

std::vector<AbstractPacket*> UpnpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
