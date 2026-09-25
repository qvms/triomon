#include "KerberosPacket.hpp"
#include <QDebug>
#include <set>

namespace PacketParser {
namespace Packets {

KerberosPacket::KerberosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool packetHasLenghtFieldHeader)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Kerberos")
    , m_msgType(MessageType::krb_null)
{
    int index = packetStartIndex;
    if (packetHasLenghtFieldHeader) {
        if (packetStartIndex + 4 <= static_cast<int>(parentFrame->DataLength())) {
            int length = static_cast<int>(Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex, 4) & 0x7fffff);
            if (packetStartIndex + 4 + length > packetEndIndex + 1) {
                if (!ParentFrame()->QuickParse()) {
                    const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetEndIndex, packetEndIndex, "Kerberos packet is truncated"));
                }
                return;
            }
            setPacketEndIndex(packetStartIndex + 4 + length - 1);
            index += 4;
        } else {
            return;
        }
    }
    
    // Read ASN.1 data
    m_asnData = Utils::ByteConverter::GetAsn1Data(parentFrame->Data(), parentFrame->DataLength(), index, packetEndIndex);
    
    std::vector<std::tuple<QString, Utils::ByteConverter::Asn1Tag, std::vector<uint8_t>>> firstInts;
    for (const auto& item : m_asnData) {
        if (std::get<1>(item) == Utils::ByteConverter::Asn1Tag::Integer) {
            firstInts.push_back(item);
            if (firstInts.size() == 2) break;
        }
    }
    
    if (firstInts.size() == 2) {
        const auto& firstData = std::get<2>(firstInts[0]);
        if (!firstData.empty() && Utils::ByteConverter::ToUInt32(firstData.data(), 0, firstData.size()) == 5) {
            const auto& secondData = std::get<2>(firstInts[1]);
            if (!secondData.empty()) {
                uint32_t commandType = Utils::ByteConverter::ToUInt32(secondData.data(), 0, secondData.size());
                // In a full implementation we'd check Enum.IsDefined. For now we just cast.
                m_msgType = static_cast<MessageType>(commandType);
            }
        } else {
            if (!ParentFrame()->QuickParse()) {
                const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, "Not Kerberos v5"));
            }
        }
    } else {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, "Not Kerberos v5"));
        }
    }
}

bool KerberosPacket::IsRequest() const {
    static const std::set<MessageType> REQUEST_TYPES = {
        MessageType::krb_as_req,
        MessageType::krb_tgs_req,
        MessageType::krb_ap_req
    };
    return REQUEST_TYPES.find(m_msgType) != REQUEST_TYPES.end();
}

std::vector<AbstractPacket*> KerberosPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
