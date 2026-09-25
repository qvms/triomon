#include "TftpPacket.hpp"
#include "../Utils.hpp"
#include <stdexcept>

namespace PacketParser {
namespace Packets {

TftpPacket::TftpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t blksize)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TFTP")
    , m_blksize(blksize)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 2 > static_cast<int>(dataLen)) return;

    m_opCode = Utils::ByteConverter::ToUInt16(data, packetStartIndex);

    if (m_opCode < static_cast<uint16_t>(OpCodes::ReadRequest) || m_opCode > static_cast<uint16_t>(OpCodes::OptionAcknowledgment)) {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex + 1, "Incorrect OPCODE, not correct TFTP packet"));
        }
        return;
    }

    if (m_opCode == static_cast<uint16_t>(OpCodes::ReadRequest) || m_opCode == static_cast<uint16_t>(OpCodes::WriteRequest)) {
        int index = packetStartIndex + 2;
        m_filename = Utils::ByteConverter::ReadNullTerminatedString(data, index);

        QString strMode = Utils::ByteConverter::ReadNullTerminatedString(data, index);
        strMode = strMode.toLower();
        if (strMode == "netascii") m_mode = Modes::netascii;
        else if (strMode == "octet") m_mode = Modes::octet;
        else if (strMode == "mail") m_mode = Modes::mail;
        else m_mode = Modes::octet; // fallback

        while (index < packetEndIndex && index < static_cast<int>(dataLen)) {
            QString optName = Utils::ByteConverter::ReadNullTerminatedString(data, index, false, false, packetEndIndex - index);
            if (optName.isNull() || index >= packetEndIndex) break;
            index += optName.length() + 1;

            QString optValue = Utils::ByteConverter::ReadNullTerminatedString(data, index, false, false, packetEndIndex - index);
            if (optValue.isNull() || index >= packetEndIndex) break;
            index += optValue.length() + 1;

            m_rfc2347OptionList[optName] = optValue;
            if (optName.compare("blksize", Qt::CaseInsensitive) == 0) {
                m_blksize = optValue.toUShort();
            }
        }
    } else if (m_opCode == static_cast<uint16_t>(OpCodes::Data)) {
        m_dataBlockNumber = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
        int copyLen = std::min(static_cast<int>(m_blksize), packetEndIndex - packetStartIndex - 3);
        if (copyLen > 0 && packetStartIndex + 4 + copyLen <= static_cast<int>(dataLen)) {
            m_dataBlock.assign(data + packetStartIndex + 4, data + packetStartIndex + 4 + copyLen);
        }
    } else if (m_opCode == static_cast<uint16_t>(OpCodes::Acknowledgment)) {
        m_dataBlockNumber = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    } else if (m_opCode == static_cast<uint16_t>(OpCodes::OptionAcknowledgment)) {
        int index = packetStartIndex + 2;
        while (index < packetEndIndex && index < static_cast<int>(dataLen)) {
            QString optName = Utils::ByteConverter::ReadNullTerminatedString(data, index, false, false, packetEndIndex - index);
            if (optName.isNull() || index >= packetEndIndex) break;
            index += optName.length() + 1;

            QString optValue = Utils::ByteConverter::ReadNullTerminatedString(data, index, false, false, packetEndIndex - index);
            if (optValue.isNull() || index >= packetEndIndex) break;
            index += optValue.length() + 1;

            m_rfc2347OptionList[optName] = optValue;
            if (optName.compare("blksize", Qt::CaseInsensitive) == 0) {
                m_blksize = optValue.toUShort();
            }
        }
    }
}

std::vector<AbstractPacket*> TftpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
