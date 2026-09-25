#include "AbstractPacket.hpp"
#include <stdexcept>
#include <QDebug>

namespace PacketParser {
namespace Packets {

AbstractPacket::AbstractPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const QString& packetTypeDescription)
    : m_parentFrame(parentFrame)
    , m_packetTypeDescription(packetTypeDescription)
    , m_packetStartIndex(packetStartIndex)
    , m_packetEndIndex(packetEndIndex)
{
    if (!parentFrame->QuickParse()) {
        if (packetStartIndex > packetEndIndex) {
            QString errorMsg = QString("PacketStartIndex (%1) > PacketEndIndex (%2)").arg(packetStartIndex).arg(packetEndIndex);
            
            // Cast away constness to add error (or use mutable in Frame if preferred, but doing const_cast here to strictly match C# logic)
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetEndIndex, packetEndIndex, errorMsg));
            
            qWarning() << "AbstractPacket initialization error:" << errorMsg;
            // Returning early or returning invalid state rather than throwing to adhere to no-exceptions rule for max performance
        }
    }
}

std::vector<uint8_t> AbstractPacket::GetPacketData() const {
    int byteCount = PacketByteCount();
    if (byteCount <= 0 || m_packetStartIndex + byteCount > m_parentFrame->DataLength()) {
        return {};
    }
    
    std::vector<uint8_t> data(byteCount);
    std::memcpy(data.data(), m_parentFrame->Data() + m_packetStartIndex, byteCount);
    return data;
}

} // namespace Packets
} // namespace PacketParser
