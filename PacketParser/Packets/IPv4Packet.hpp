#pragma once
#include "AbstractPacket.hpp"
#include "IIPPacket.hpp"
#include "../PopularityList.hpp"
#include <QString>
#include <cstdint>
#include <vector>
#include <mutex>

namespace PacketParser {
namespace Packets {

class IPv4Packet : public AbstractPacket, public virtual IIPPacket {
private:
    static constexpr uint16_t FRAGMENT_OFFSET_MASK = 0x1fff;
    static constexpr uint16_t MAX_JUMBO_FRAME_SIZE = 9038;

    QString m_sourceIP;
    QString m_destinationIP;
    bool m_dontFragmentFlag;
    bool m_moreFragmentsFlag;
    uint16_t m_fragmentOffset;
    uint16_t m_identification;
    uint8_t m_timeToLive;
    uint8_t m_protocol;
    uint8_t m_headerLength;
    uint16_t m_totalLength;

    // A static pointer to a popular list could be used to mimic the static property in C#
    // but in Qt/C++ this might be better handled by a state manager.
    // For 1:1 parity:
    static PopularityList<QString, std::vector<IPv4Packet*>>* s_Ipv4Fragments;
    static std::mutex s_Ipv4FragmentsMutex;

public:
    static PopularityList<QString, std::vector<IPv4Packet*>>* Ipv4Fragments() { return s_Ipv4Fragments; }
    static void SetIpv4Fragments(PopularityList<QString, std::vector<IPv4Packet*>>* list) { s_Ipv4Fragments = list; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result);
    static bool TryGetSubPacket(uint8_t rfc1700Protocol, const Frame* parentFrame, int startIndex, int endIndex, AbstractPacket*& subPacket);

    IPv4Packet(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    QString SourceIPAddress() const override { return m_sourceIP; }
    QString DestinationIPAddress() const override { return m_destinationIP; }
    uint8_t HeaderLength() const override { return m_headerLength; }
    uint16_t TotalLength() const { return m_totalLength; }
    int PayloadLength() const override { return m_totalLength - m_headerLength; }
    bool DontFragmentFlag() const { return m_dontFragmentFlag; }
    uint8_t TimeToLive() const { return m_timeToLive; }
    uint8_t HopLimit() const override { return m_timeToLive; }
    uint8_t NextRFC1700Protocol() const override { return m_protocol; }

    QString GetFragmentIdentifier() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
