#include "OscarFileTransferPacket.hpp"

namespace PacketParser {
namespace Packets {

OscarFileTransferPacket::OscarFileTransferPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "OSCAR File Transfer")
{
    m_commandType = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 6);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Command Type", "0x" + QString::number(m_commandType, 16).rightJustified(2, '0').toUpper());
    }
    
    m_totalFileSize = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 28);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Total File Size", QString::number(m_totalFileSize));
    }
    
    int index = packetStartIndex + 68;
    QString idString = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index);
    
    index = packetStartIndex + 192;
    m_fileName = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Filename", m_fileName);
    }
}

bool OscarFileTransferPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    try {
        if (packetEndIndex - packetStartIndex < 255) return false;
        if (Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, 4, false) != "OFT2") return false;
        
        uint16_t cmdType = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 6);
        if (cmdType != 0x0101 && cmdType != 0x0202 && cmdType != 0x0204) return false;
        
        *result = new OscarFileTransferPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<AbstractPacket*> OscarFileTransferPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
