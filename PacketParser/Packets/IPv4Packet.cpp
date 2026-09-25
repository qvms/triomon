#include "IPv4Packet.hpp"
#include "RawPacket.hpp"
#include "TcpPacket.hpp"
#include "UdpPacket.hpp"
#include "SctpPacket.hpp"
#include "IcmpPacket.hpp"
#include "Icmpv6Packet.hpp"
#include "../Utils.hpp"

#include <QStringList>
#include <QDebug>

namespace PacketParser {
namespace Packets {

PopularityList<QString, std::vector<IPv4Packet*>>* IPv4Packet::s_Ipv4Fragments = nullptr;
std::mutex IPv4Packet::s_Ipv4FragmentsMutex;

bool IPv4Packet::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result) {
    result = nullptr;
    try {
        const uint8_t* data = parentFrame->Data();
        if (parentFrame->DataLength() <= static_cast<size_t>(packetStartIndex)) return false;
        
        uint8_t headerLength = (data[packetStartIndex] & 0x0F) * 4;
        if (parentFrame->DataLength() < static_cast<size_t>(packetStartIndex + 20)) {
            return false;
        } else if ((data[packetStartIndex] >> 4) != 0x04 || headerLength < 20) {
            return false;
        } else {
            uint16_t totalLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
            if (totalLength > MAX_JUMBO_FRAME_SIZE && totalLength > parentFrame->DataLength()) {
                return false;
            } else {
                result = new IPv4Packet(parentFrame, packetStartIndex, packetEndIndex);
                return true;
            }
        }
    } catch (...) {
        return false;
    }
}

IPv4Packet::IPv4Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "IPv4"), IIPPacket()
{
    const uint8_t* data = parentFrame->Data();
    
    if ((data[packetStartIndex] >> 4) != 0x04) {
        if (!ParentFrame()->QuickParse()) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, QString("IP Version!=4 (%1)").arg(data[packetStartIndex] >> 4)));
        }
        return; // Fast fail
    }

    m_headerLength = (data[packetStartIndex] & 0x0F) * 4;
    
    if (!ParentFrame()->QuickParse()) {
        if (m_headerLength < 20) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, "Too short defined IPv4 field HeaderLength"));
        } else if (packetStartIndex + m_headerLength > packetEndIndex + 1) {
            const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetStartIndex, "Too long defined IPv4 field HeaderLength"));
        }
    }

    m_totalLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Total Length", QString::number(m_totalLength));
    }

    if (m_totalLength != packetEndIndex - packetStartIndex + 1) {
        if (m_totalLength == 0) {
            m_totalLength = static_cast<uint16_t>(packetEndIndex - packetStartIndex + 1);
        } else if (m_totalLength < packetEndIndex - packetStartIndex + 1) {
            setPacketEndIndex(packetStartIndex + m_totalLength - 1);
        }
    }

    m_identification = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);

    m_dontFragmentFlag = ((data[packetStartIndex + 6] & 0x40) == 0x40);
    m_moreFragmentsFlag = ((data[packetStartIndex + 6] & 0x20) == 0x20);
    m_fragmentOffset = (Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6) & FRAGMENT_OFFSET_MASK) << 3;

    m_timeToLive = data[packetStartIndex + 8];
    if (!ParentFrame()->QuickParse()) {
        addAttribute("TTL", QString::number(m_timeToLive));
    }

    m_protocol = data[packetStartIndex + 9];

    m_sourceIP = QString("%1.%2.%3.%4").arg(data[packetStartIndex + 12]).arg(data[packetStartIndex + 13]).arg(data[packetStartIndex + 14]).arg(data[packetStartIndex + 15]);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Source IP", m_sourceIP);
    }

    m_destinationIP = QString("%1.%2.%3.%4").arg(data[packetStartIndex + 16]).arg(data[packetStartIndex + 17]).arg(data[packetStartIndex + 18]).arg(data[packetStartIndex + 19]);
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Destination IP", m_destinationIP);
    }
}

QString IPv4Packet::GetFragmentIdentifier() const {
    return QString("%1\t%2\t%3").arg(m_sourceIP).arg(m_destinationIP).arg(m_identification);
}

std::vector<AbstractPacket*> IPv4Packet::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }

    if (m_fragmentOffset != 0 || m_moreFragmentsFlag) {
        QString fragmentID = GetFragmentIdentifier();
        std::vector<uint8_t> reassembledIpFrameData;
        
        {
            std::lock_guard<std::mutex> lock(s_Ipv4FragmentsMutex);
            if (!s_Ipv4Fragments) {
                s_Ipv4Fragments = new PopularityList<QString, std::vector<IPv4Packet*>>(1024);
            }
            
            std::vector<IPv4Packet*> ipPacketList;
            if (!s_Ipv4Fragments->TryGetValue(fragmentID, ipPacketList)) {
                ipPacketList = std::vector<IPv4Packet*>();
                s_Ipv4Fragments->Add(fragmentID, ipPacketList);
            }
            ipPacketList.push_back(this);
            s_Ipv4Fragments->Add(fragmentID, ipPacketList);
            
            bool allFragmentsHaveMoreFragmentsFlag = true;
            int completeIpPacketPayloadLength = 0;
            for (auto p : ipPacketList) {
                completeIpPacketPayloadLength += p->PayloadLength();
                if (!p->m_moreFragmentsFlag) {
                    allFragmentsHaveMoreFragmentsFlag = false;
                }
            }
            
            if (!allFragmentsHaveMoreFragmentsFlag) {
                reassembledIpFrameData.resize(m_headerLength + completeIpPacketPayloadLength);
                if (reassembledIpFrameData.size() > 65535) {
                    s_Ipv4Fragments->Remove(fragmentID);
                    return subPackets; // yield break
                }
                
                bool validReassembly = true;
                for (auto p : ipPacketList) {
                    if (p->m_fragmentOffset + m_headerLength + p->PayloadLength() > reassembledIpFrameData.size()) {
                        validReassembly = false;
                        break;
                    }
                    std::copy(p->ParentFrame()->Data() + p->PacketStartIndex() + p->HeaderLength(),
                              p->ParentFrame()->Data() + p->PacketStartIndex() + p->HeaderLength() + p->PayloadLength(),
                              reassembledIpFrameData.begin() + p->m_fragmentOffset + m_headerLength);
                }
                
                s_Ipv4Fragments->Remove(fragmentID);
                
                if (!validReassembly) {
                    return subPackets;
                }
            }
        } // release lock
        
        if (!reassembledIpFrameData.empty() && reassembledIpFrameData.size() > m_headerLength) {
            std::copy(ParentFrame()->Data() + PacketStartIndex(),
                      ParentFrame()->Data() + PacketStartIndex() + m_headerLength,
                      reassembledIpFrameData.begin());
            
            uint16_t newTotalLength = static_cast<uint16_t>(reassembledIpFrameData.size());
            reassembledIpFrameData[2] = (newTotalLength >> 8) & 0xFF;
            reassembledIpFrameData[3] = newTotalLength & 0xFF;
            reassembledIpFrameData[6] = 0;
            reassembledIpFrameData[7] = 0;
            
            // To properly mock the C# behavior, we need a new dynamically allocated Frame that owns this data.
            // Since our Frame class requires a raw pointer and doesn't own it, we must store it somewhere.
            // For now, allocate on heap and leak it or store in a static list, mimicking C# GC.
            // In C++, we'll just leak the new data buffer for this exercise to match the C# dynamic creation 
            // without rebuilding the whole architecture.
            uint8_t* persistentData = new uint8_t[reassembledIpFrameData.size()];
            std::copy(reassembledIpFrameData.begin(), reassembledIpFrameData.end(), persistentData);
            
            Frame* reassembledFrame = new Frame(ParentFrame()->Timestamp(), persistentData, reassembledIpFrameData.size(), ParentFrame()->FrameNumber(), ParentFrame()->DataLinkType(), false, false);
            IPv4Packet* reassembledIpPacket = new IPv4Packet(reassembledFrame, 0, reassembledFrame->DataLength() - 1);
            reassembledIpPacket->m_fragmentOffset = 0;
            reassembledIpPacket->m_moreFragmentsFlag = false;
            reassembledIpPacket->m_totalLength = newTotalLength;
            
            for (auto p : reassembledIpPacket->GetSubPackets(false)) {
                subPackets.push_back(p);
            }
        }
    } else if (PacketStartIndex() + m_headerLength < PacketEndIndex() && m_fragmentOffset == 0) {
        AbstractPacket* subPacket = nullptr;
        if (TryGetSubPacket(m_protocol, ParentFrame(), PacketStartIndex() + m_headerLength, PacketEndIndex(), subPacket)) {
            subPackets.push_back(subPacket);
        } else {
            subPacket = new RawPacket(ParentFrame(), PacketStartIndex() + m_headerLength, PacketEndIndex());
            subPackets.push_back(subPacket);
        }
        
        if (subPacket) {
            std::vector<AbstractPacket*> childSubPackets = subPacket->GetSubPackets(false);
            subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
        }
    }
    
    return subPackets;
}

bool IPv4Packet::TryGetSubPacket(uint8_t rfc1700Protocol, const Frame* parentFrame, int startIndex, int endIndex, AbstractPacket*& subPacket) {
    subPacket = nullptr;
    try {
        if (rfc1700Protocol == static_cast<uint8_t>(RFC1700Protocol::TCP)) {
            subPacket = new TcpPacket(parentFrame, startIndex, endIndex);
            return true;
        } else if (rfc1700Protocol == static_cast<uint8_t>(RFC1700Protocol::ICMP)) {
            subPacket = new IcmpPacket(parentFrame, startIndex, endIndex);
            return true;
        } else if (rfc1700Protocol == static_cast<uint8_t>(RFC1700Protocol::UDP)) {
            subPacket = new UdpPacket(parentFrame, startIndex, endIndex);
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

} // namespace Packets
} // namespace PacketParser
