#include "UdpPacket.hpp"
#include "RawPacket.hpp"
#include "DnsPacket.hpp"
#include "DhcpPacket.hpp"
#include "TftpPacket.hpp"
#include "NetBiosNameServicePacket.hpp"
#include "NetBiosDatagramServicePacket.hpp"
#include "SyslogPacket.hpp"
#include "UpnpPacket.hpp"
#include "RtpPacket.hpp"
#include "SipPacket.hpp"
#include "SnmpPacket.hpp"
#include "TeredoPacket.hpp"
#include "GtpPacket.hpp"
#include "VxlanPacket.hpp"
#include "CapwapPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

UdpPacket::UdpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "UDP")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLength = parentFrame->DataLength();

    if (packetStartIndex + 8 > static_cast<int>(dataLength)) return;

    m_sourcePort = Utils::ByteConverter::ToUInt16(data, packetStartIndex);
    if (!ParentFrame()->QuickParse()) addAttribute("Source Port", QString::number(m_sourcePort));

    m_destinationPort = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    if (!ParentFrame()->QuickParse()) addAttribute("Destination Port", QString::number(m_destinationPort));

    m_length = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);

    if (m_length != packetEndIndex - packetStartIndex + 1) {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 4, PacketStartIndex() + 5, QString("UDP defined length (%1) differs from actual length (%2)").arg(m_length).arg(packetEndIndex - packetStartIndex + 1)));
        }
        if (packetEndIndex > packetStartIndex + m_length - 1) {
            setPacketEndIndex(packetStartIndex + m_length - 1);
        }
    }

    m_checksum = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
}

std::vector<AbstractPacket*> UdpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + 8 < PacketEndIndex()) {

        AbstractPacket* subPacket = nullptr;

        if (m_sourcePort == 53 || m_destinationPort == 53) {
            subPacket = new DnsPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        } else if (m_sourcePort == 67 || m_destinationPort == 67 || m_sourcePort == 68 || m_destinationPort == 68) {
            subPacket = new DhcpPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        } else if (m_sourcePort == 69 || m_destinationPort == 69) {
            subPacket = new TftpPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        } else if (m_sourcePort == 137 || m_destinationPort == 137) {
            if (!NetBiosNameServicePacket::TryParse(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex(), subPacket)) {
                subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
            }
                } else if (m_sourcePort == 138 || m_destinationPort == 138) {
            subPacket = new NetBiosDatagramServicePacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        } else if (m_sourcePort == 5246 || m_destinationPort == 5246 || m_sourcePort == 5247 || m_destinationPort == 5247) {
            subPacket = new CapwapPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        } else {
            subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        }

        if (subPacket) {
            subPackets.push_back(subPacket);
            std::vector<AbstractPacket*> childSubPackets = subPacket->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }

    }

    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
