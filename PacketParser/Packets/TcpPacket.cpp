#include "TcpPacket.hpp"
#include "RawPacket.hpp"
#include "TpktPacket.hpp"
#include "DnsPacket.hpp"
#include "NetBiosSessionService.hpp"
#include "FtpPacket.hpp"
#include "Pop3Packet.hpp"
#include "SmtpPacket.hpp"
#include "HttpPacket.hpp"
#include "RmsPacket.hpp"
#include "RemcosPacket.hpp"
#include "MeterpreterPacket.hpp"
#include "SpotifyKeyExchangePacket.hpp"
#include "OscarFileTransferPacket.hpp"
#include "OscarPacket.hpp"
#include "McNmfPacket.hpp"
#include "OpenFlowPacket.hpp"
#include "LpdPacket.hpp"
#include "CotpPacket.hpp"
#include "UmasPacket.hpp"
#include "EtherNetIPPacket.hpp"
#include "Http2Packet.hpp"
#include "SipPacket.hpp"
#include "KerberosPacket.hpp"
#include "SslPacket.hpp"
#include "SshPacket.hpp"
#include "ImapPacket.hpp"
#include "IrcPacket.hpp"
#include "TabularDataStreamPacket.hpp"
#include "../Utils.hpp"
#include <QStringList>

namespace PacketParser {
namespace Packets {

QString TcpPacket::Flags::ToString() const {
    QStringList parts;
    if (CongestionWindowReduced()) parts << "CWR";
    if (ECNEcho()) parts << "ECN-Echo";
    if (UrgentPointer()) parts << "Urgent";
    if (Acknowledgement()) parts << "ACK";
    if (Push()) parts << "Push";
    if (Reset()) parts << "Reset";
    if (Synchronize()) parts << "SYN";
    if (Fin()) parts << "FIN";
    return parts.join(" ");
}

TcpPacket::TcpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TCP")
    , m_flags(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t len = parentFrame->DataLength();

    if (packetStartIndex + 20 > static_cast<int>(len)) return;

    m_sourcePort = Utils::ByteConverter::ToUInt16(data, packetStartIndex);
    if (!ParentFrame()->QuickParse()) addAttribute("Source Port", QString::number(m_sourcePort));

    m_destinationPort = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    if (!ParentFrame()->QuickParse()) addAttribute("Destination Port", QString::number(m_destinationPort));

    m_sequenceNumber = (data[packetStartIndex + 4] << 24) | (data[packetStartIndex + 5] << 16) | (data[packetStartIndex + 6] << 8) | data[packetStartIndex + 7];
    if (!ParentFrame()->QuickParse()) addAttribute("Sequence Number", QString("%1").arg(m_sequenceNumber, 8, 16, QChar('0')).toUpper());

    m_acknowledgmentNumber = (data[packetStartIndex + 8] << 24) | (data[packetStartIndex + 9] << 16) | (data[packetStartIndex + 10] << 8) | data[packetStartIndex + 11];

    m_dataOffsetByteCount = (data[packetStartIndex + 12] >> 4) * 4;

    if (!ParentFrame()->QuickParse()) {
        if (m_dataOffsetByteCount < 20) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 12, packetStartIndex + 12, QString("Too small defined TCP Data Offset : %1").arg(data[packetStartIndex + 12])));
        } else if (m_dataOffsetByteCount > 60) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 12, packetStartIndex + 12, QString("Too large defined TCP Data Offset : %1").arg(data[packetStartIndex + 12])));
        } else if (PacketEndIndex() - PacketStartIndex() + 1 < m_dataOffsetByteCount) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex + 12, packetStartIndex + 12, "TCP Data offset is outside frame"));
        }
    }

    m_flags = Flags(data[packetStartIndex + 13]);
    if (!ParentFrame()->QuickParse()) addAttribute("Flags", m_flags.ToString());

    m_windowSize = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 14);
    m_checksum = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 16);

    if (m_dataOffsetByteCount > 20) {
        m_optionList = GetOptionList(packetStartIndex + 20);
    }
}

std::vector<std::pair<TcpPacket::OptionKinds, std::vector<uint8_t>>> TcpPacket::GetOptionList(int optionStartIndex) {
    std::vector<std::pair<OptionKinds, std::vector<uint8_t>>> list;
    const uint8_t* data = ParentFrame()->Data();
    int currentIndex = optionStartIndex;
    int endIndex = PacketStartIndex() + m_dataOffsetByteCount;

    while (currentIndex < endIndex && currentIndex < static_cast<int>(ParentFrame()->DataLength())) {
        OptionKinds kind = static_cast<OptionKinds>(data[currentIndex]);
        if (kind == OptionKinds::EndOfOptionList || kind == OptionKinds::NoOperation) {
            list.push_back({kind, {}});
            currentIndex++;
        } else {
            if (currentIndex + 1 >= endIndex) break; // Malformed option
            uint8_t optLen = data[currentIndex + 1];
            if (optLen < 2 || currentIndex + optLen > endIndex) break; // Malformed or out of bounds
            
            std::vector<uint8_t> optData(optLen - 2);
            if (optLen > 2) {
                std::memcpy(optData.data(), data + currentIndex + 2, optLen - 2);
            }
            list.push_back({kind, optData});
            currentIndex += optLen;
        }
    }
    return list;
}

std::vector<AbstractPacket*> TcpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + m_dataOffsetByteCount < PacketEndIndex()) {
        AbstractPacket* subPacket = nullptr;

        if (m_sourcePort == 53 || m_destinationPort == 53) {
            subPacket = new DnsPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount + 2, PacketEndIndex());
        } else if (m_destinationPort == 1433 || m_sourcePort == 1433) {
            subPacket = new TabularDataStreamPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
        } else if (NetBiosSessionService::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), m_sourcePort, m_destinationPort, subPacket, false)) {
            // NetBiosSessionService parsing succeeded
        } else if (FtpPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), m_destinationPort == 21, m_sourcePort, subPacket)) {
            // FtpPacket parsing succeeded
        } else if (m_destinationPort == 110 || m_sourcePort == 110) {
            Pop3Packet::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), m_destinationPort == 110, subPacket);
            if (!subPacket) subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
        } else if (m_destinationPort == 25 || m_sourcePort == 25 || m_destinationPort == 587 || m_sourcePort == 587) {
            SmtpPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), m_destinationPort == 25 || m_destinationPort == 587, m_sourcePort, subPacket);
            if (!subPacket) subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
        } else if (m_destinationPort == 143 || m_sourcePort == 143) {
            ImapPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), m_destinationPort == 143, subPacket);
            if (!subPacket) subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
        } else if (m_destinationPort == 6667 || m_sourcePort == 6667) {
            if (!IrcPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), subPacket)) {
                subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
            }
        } else if (HttpPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), subPacket)) {
            // HttpPacket parsing succeeded
        } else if (!TpktPacket::TryParse(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex(), this, subPacket)) {
            subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_dataOffsetByteCount, PacketEndIndex());
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
