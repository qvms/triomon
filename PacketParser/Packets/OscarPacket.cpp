#include "OscarPacket.hpp"
#include "../Utils.hpp"
#include <cstring>
#include <QDebug>

namespace PacketParser {
namespace Packets {

OscarPacket::TagLengthValue::TagLengthValue(const uint8_t* data, size_t dataLen, int& offset) {
    if (offset + 4 <= static_cast<int>(dataLen)) {
        Tag = Utils::ByteConverter::ToUInt16(data, offset);
        offset += 2;
        Length = Utils::ByteConverter::ToUInt16(data, offset);
        offset += 2;
        if (offset + Length <= static_cast<int>(dataLen)) {
            Value.resize(Length);
            std::memcpy(Value.data(), data + offset, Length);
            offset += Length;
        }
    }
}

OscarPacket::TagLengthValue::TagLengthValue(const std::vector<uint8_t>& data, int& offset) {
    if (offset + 4 <= static_cast<int>(data.size())) {
        Tag = Utils::ByteConverter::ToUInt16(data.data(), offset);
        offset += 2;
        Length = Utils::ByteConverter::ToUInt16(data.data(), offset);
        offset += 2;
        if (offset + Length <= static_cast<int>(data.size())) {
            Value.resize(Length);
            std::memcpy(Value.data(), data.data() + offset, Length);
            offset += Length;
        }
    }
}

OscarPacket::OscarPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "OSCAR")
    , m_isValid(true)
{
    int index = packetStartIndex;
    index++; // skip 0x2a
    m_flapChannel = parentFrame->Data()[index++];
    m_seqNumber = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
    index += 2;
    m_bytesToParse = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
    index += 2;
    
    if (index + m_bytesToParse - 1 < packetEndIndex) {
        setPacketEndIndex(index + m_bytesToParse - 1);
    }
    
    if (m_flapChannel == 2 && m_bytesToParse >= 10 && index + 10 <= packetEndIndex + 1) {
        uint16_t family = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
        uint16_t type = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index + 2);
        
        m_snacFamily = family;
        m_snacSubType = type;
        
        uint16_t flags = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index + 4);
        uint32_t reqId = Utils::ByteConverter::ToUInt32(parentFrame->Data(), index + 6);
        index += 10;
        
        if (family == 4) { // ICBM
            if (type == 6) { // ICBM__CHANNEL_MSG_TOHOST
                index += 8; // skip 8 bytes cookie
                uint16_t channel = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
                index += 2;
                m_destinationLoginId = Utils::ByteConverter::ReadLengthValueString(parentFrame->Data(), parentFrame->DataLength(), index, 1);
                
                while (index < parentFrame->DataLength() && index <= PacketEndIndex() && index < packetStartIndex + 6 + m_bytesToParse) {
                    TagLengthValue tlv(parentFrame->Data(), parentFrame->DataLength(), index);
                    if (tlv.Tag == static_cast<uint16_t>(TagLengthValue::IcbmTag::IM_DATA)) {
                        int tlvIndex = 0;
                        while (tlvIndex < tlv.Length && tlvIndex + 4 <= tlv.Value.size()) {
                            TagLengthValue dataTlv(tlv.Value, tlvIndex);
                            if (dataTlv.Tag == static_cast<uint16_t>(TagLengthValue::IcbmImDataTag::IM_TEXT) && dataTlv.Length >= 4) {
                                m_imText = Utils::ByteConverter::ReadString(dataTlv.Value.data(), dataTlv.Value.size(), 4, dataTlv.Length - 4, false);
                                if (!ParentFrame()->QuickParse()) addAttribute("IM Text", m_imText);
                            }
                        }
                    }
                }
            } else if (type == 7) { // ICBM__CHANNEL_MSG_TOCLIENT
                index += 8; // skip cookie
                uint16_t channel = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
                index += 2;
                m_sourceLoginId = Utils::ByteConverter::ReadLengthValueString(parentFrame->Data(), parentFrame->DataLength(), index, 1);
                index += 2; // evil
                uint16_t nBlocks = Utils::ByteConverter::ToUInt16(parentFrame->Data(), index);
                index += 2;
                for (int i = 0; i < nBlocks && index < ParentFrame()->DataLength(); i++) {
                    TagLengthValue tlv(parentFrame->Data(), parentFrame->DataLength(), index);
                }
                while (index < parentFrame->DataLength() && index <= PacketEndIndex() && index < packetStartIndex + 6 + m_bytesToParse) {
                    TagLengthValue tlv(parentFrame->Data(), parentFrame->DataLength(), index);
                    if (tlv.Tag == static_cast<uint16_t>(TagLengthValue::IcbmTag::IM_DATA)) {
                        int tlvIndex = 0;
                        while (tlvIndex < tlv.Length && tlvIndex + 4 <= tlv.Value.size()) {
                            TagLengthValue dataTlv(tlv.Value, tlvIndex);
                            if (dataTlv.Tag == static_cast<uint16_t>(TagLengthValue::IcbmImDataTag::IM_TEXT) && dataTlv.Length >= 4) {
                                m_imText = Utils::ByteConverter::ReadString(dataTlv.Value.data(), dataTlv.Value.size(), 4, dataTlv.Length - 4, false);
                                if (!ParentFrame()->QuickParse()) addAttribute("IM Text", m_imText);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool OscarPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    if (packetEndIndex - packetStartIndex < 6) return false;
    if (parentFrame->Data()[packetStartIndex] != 0x2a) return false;
    
    uint16_t len = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 4);
    if (len > packetEndIndex - packetStartIndex + 1 - 6) return false;
    
    try {
        *result = new OscarPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<AbstractPacket*> OscarPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
