#include "CotpPacket.hpp"
#include <QDebug>
#include <cstring>

namespace PacketParser {
namespace Packets {

CotpPacket::CotpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, int encapsulatedProtocol)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "ISO/IEC 8073/X.224 COTP")
    , m_length(0)
    , m_pduType(0)
    , m_tpduNumber(0)
    , m_pduClass(0)
    , m_endOfTsdu(false)
    , EncapsulatedProtocol(encapsulatedProtocol)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 2 < static_cast<int>(dataLen)) {
        m_length = data[packetStartIndex];
        m_pduType = data[packetStartIndex + 1];
        m_tpduNumber = static_cast<uint8_t>(data[packetStartIndex + 2] & 0x7f);
        m_endOfTsdu = data[packetStartIndex + 2] >= 0x80;

        if (m_endOfTsdu && GetTpdu() != Tpdu::Data) {
            if (packetStartIndex + 6 < static_cast<int>(dataLen)) {
                m_pduClass = static_cast<uint8_t>(data[packetStartIndex + 6] & 0xf0);
            }
        }
    }
}

CotpPacket::Tpdu CotpPacket::GetTpdu() const {
    uint8_t val = static_cast<uint8_t>(m_pduType >> 4);
    switch (val) {
        case 0xe: return Tpdu::ConnectionRequest;
        case 0xd: return Tpdu::ConnectionConfirm;
        case 0x8: return Tpdu::DisconnectRequest;
        case 0xc: return Tpdu::DisconnectConfirm;
        case 0xf: return Tpdu::Data;
        case 0x1: return Tpdu::ExpeditedData;
        case 0x6: return Tpdu::DataAcknowledge;
        case 0x2: return Tpdu::ExpeditedAcknowledge;
        case 0x5: return Tpdu::Reject;
        case 0x7: return Tpdu::Error;
        default: return Tpdu::UNKNOWN;
    }
}

bool CotpPacket::TryGetTsduData(std::vector<uint8_t>& data) const {
    if (GetTpdu() == Tpdu::Data) {
        int length = PacketLength() - 3;
        if (length > 0 && PacketStartIndex() + 3 + length <= static_cast<int>(ParentFrame()->DataLength())) {
            data.resize(length);
            std::memcpy(data.data(), ParentFrame()->Data() + PacketStartIndex() + 3, length);
            return true;
        }
    }
    return false;
}

bool CotpPacket::TryGetVariablePartIndex(int& variablePartIndex) const {
    variablePartIndex = -1;
    Tpdu tpdu = GetTpdu();

    if (tpdu == Tpdu::ConnectionRequest) {
        variablePartIndex = PacketStartIndex() + 7;
    } else if (tpdu == Tpdu::ConnectionConfirm) {
        variablePartIndex = PacketStartIndex() + 7;
    } else if (tpdu == Tpdu::DisconnectConfirm) {
        variablePartIndex = PacketStartIndex() + 6;
    } else if (tpdu == Tpdu::Data) {
        if (m_pduClass >= 2 && m_pduClass < 5) {
            variablePartIndex = PacketStartIndex() + 5;
        }
    }

    return variablePartIndex > 0;
}

std::vector<AbstractPacket*> CotpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
