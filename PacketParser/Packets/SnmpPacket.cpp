#include "SnmpPacket.hpp"
#include <set>

namespace PacketParser {
namespace Packets {

SnmpPacket::SnmpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "SNMP")
    , VersionRaw(0)
{
    if (!ParentFrame()->QuickParse()) {
        std::vector<uint8_t> snmpAsn1Data(PacketLength());
        std::memcpy(snmpAsn1Data.data(), parentFrame->Data() + packetStartIndex, PacketLength());

        int index = 0;
        std::set<uint8_t> allowedTypes = {2}; // 2 == INTEGER
        
        std::vector<std::vector<uint8_t>> b = Utils::ByteConverter::GetAsn1DerSequenceTypes(snmpAsn1Data.data(), snmpAsn1Data.size(), index, allowedTypes);
        if (b.size() > 0 && b[0].size() == 1) {
            VersionRaw = b[0][0];
        }

        if (VersionRaw == static_cast<uint8_t>(Version::SNMPv1) || VersionRaw == static_cast<uint8_t>(Version::SNMPv2c)) {
            index = 0;
            std::vector<QString> s = Utils::ByteConverter::ReadAsn1DerSequenceStrings(snmpAsn1Data.data(), snmpAsn1Data.size(), index);
            if (s.size() > 0) {
                CommunityString = s[0];
                for (size_t i = 1; i < s.size(); ++i) {
                    CarvedStrings.append(s[i]);
                }
            }
        } else if (VersionRaw == static_cast<uint8_t>(Version::SNMPv3)) {
            index = 0;
            std::vector<QString> s = Utils::ByteConverter::ReadAsn1DerSequenceStrings(snmpAsn1Data.data(), snmpAsn1Data.size(), index);
            for (const QString& str : s) {
                CarvedStrings.append(str);
            }
        }
    }
}

std::vector<AbstractPacket*> SnmpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
