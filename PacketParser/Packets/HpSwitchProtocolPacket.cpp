#include "HpSwitchProtocolPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

HpSwitchProtocolPacket::HpSwitchProtocolPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "HP Switch Protocol")
{
    if (packetStartIndex + 1 <= packetEndIndex) {
        m_version = parentFrame->Data()[packetStartIndex];
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Version", "0x" + QString::number(m_version, 16).rightJustified(2, '0').toUpper());
        }
        
        m_type = parentFrame->Data()[packetStartIndex + 1];
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Type", "0x" + QString::number(m_type, 16).rightJustified(2, '0').toUpper());
        }
    }
}

std::vector<AbstractPacket*> HpSwitchProtocolPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    
    int fieldCount = 0;
    int dataPosition = PacketStartIndex() + 2;
    while (dataPosition < PacketEndIndex() && fieldCount < 20) {
        HpSwField* field = new HpSwField(ParentFrame(), dataPosition, PacketEndIndex());
        dataPosition += field->PacketLength();
        fieldCount++;
        subPackets.push_back(field);
    }
    
    return subPackets;
}


HpSwitchProtocolPacket::HpSwField::HpSwField(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "HP Switch Protocol Field")
{
    if (packetStartIndex + 1 <= packetEndIndex) {
        m_typeByte = parentFrame->Data()[packetStartIndex];
        m_valueLength = parentFrame->Data()[packetStartIndex + 1];
        
        int availableLength = std::min((int)m_valueLength, PacketLength() - 2);
        if (availableLength > 0 && packetStartIndex + 2 + availableLength <= static_cast<int>(parentFrame->DataLength())) {
            m_valueBytes.resize(availableLength);
            std::memcpy(m_valueBytes.data(), parentFrame->Data() + packetStartIndex + 2, availableLength);
        }
        
        if (!ParentFrame()->QuickParse()) {
            addAttribute("Field 0x" + QString::number(m_typeByte, 16).rightJustified(2, '0').toUpper(), ValueString());
        }
        
        setPacketEndIndex(packetStartIndex + 1 + m_valueLength);
    }
}

QString HpSwitchProtocolPacket::HpSwField::ValueString() const {
    int i = 0;
    return Utils::ByteConverter::ReadNullTerminatedString(m_valueBytes.data(), m_valueBytes.size(), i, false, false, m_valueLength);
}

std::vector<AbstractPacket*> HpSwitchProtocolPacket::HpSwField::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
