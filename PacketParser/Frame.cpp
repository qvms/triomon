#include "Frame.hpp"
#include "Packets/AbstractPacket.hpp"
#include <stdexcept>

#include "Packets/PacketFactory.hpp"

namespace PacketParser {

Frame::Frame(QDateTime timestamp, const uint8_t* data, size_t dataLength, int64_t frameNumber, PacketParser::DataLinkTypeEnum dataLinkType, bool precomputePacketList, bool quickParse, int maxFrameSize)
    : m_frameNumber(frameNumber),
      m_dataLinkType(dataLinkType),
      m_timestamp(timestamp),
      m_data(data),
      m_dataLength(dataLength),
      m_quickParse(quickParse),
      m_precomputePacketList(precomputePacketList)
{
    if (dataLength > static_cast<size_t>(maxFrameSize)) {
        // To avoid exceptions per user instructions, we just clip or log
        // The original C# threw ArgumentException
        // We'll proceed with clipping
        m_dataLength = maxFrameSize;
    }

    if (!quickParse) {
        // errorList already initialized by default constructor of std::vector
    }

    Packets::AbstractPacket* packet = nullptr;
    if (m_dataLength > 0) {
        Packets::PacketFactory::TryGetPacket(packet, dataLinkType, this, 0, m_dataLength - 1);
    }
    
    if (packet != nullptr) {
        m_packetList[packet->PacketStartIndex()] = packet;
        if (m_precomputePacketList) {
            for (auto p : packet->GetSubPackets(false)) {
                m_packetList[p->PacketStartIndex()] = p;
            }
        }
    }
}

Frame::~Frame() {
    for (auto const& [index, packet] : m_packetList) {
        delete packet;
    }
}

} // namespace PacketParser
