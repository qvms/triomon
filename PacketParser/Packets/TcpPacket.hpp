#pragma once
#include "AbstractPacket.hpp"
#include "ITransportLayerPacket.hpp"
#include <vector>
#include <utility>

namespace PacketParser {
namespace Packets {

class TcpPacket : public AbstractPacket, public virtual ITransportLayerPacket {
public:
    enum class OptionKinds : uint8_t {
        EndOfOptionList = 0x00,
        NoOperation = 0x01,
        MaximumSegmentSize = 0x02,
        WindowScaleFactor = 0x03,
        SackPermitted = 0x04,
        Sack = 0x05,
        Echo = 0x06,
        EchoReply = 0x07,
        Timestamp = 0x08
    };

    struct Flags {
        uint8_t flagData;

        bool CongestionWindowReduced() const { return (flagData & 0x80) == 0x80; }
        bool ECNEcho() const { return (flagData & 0x40) == 0x40; }
        bool UrgentPointer() const { return (flagData & 0x20) == 0x20; }
        bool Acknowledgement() const { return (flagData & 0x10) == 0x10; }
        bool Push() const { return (flagData & 0x08) == 0x08; }
        bool Reset() const { return (flagData & 0x04) == 0x04; }
        bool Synchronize() const { return (flagData & 0x02) == 0x02; }
        bool Fin() const { return (flagData & 0x01) == 0x01; }

        explicit Flags(uint8_t data) : flagData(data) {}
        QString ToString() const;
    };

private:
    uint16_t m_sourcePort;
    uint16_t m_destinationPort;
    uint32_t m_sequenceNumber;
    uint32_t m_acknowledgmentNumber;
    uint8_t m_dataOffsetByteCount;
    Flags m_flags;
    uint16_t m_windowSize;
    uint16_t m_checksum;
    std::vector<std::pair<OptionKinds, std::vector<uint8_t>>> m_optionList;

public:
    TcpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint16_t SourcePort() const override { return m_sourcePort; }
    uint16_t DestinationPort() const override { return m_destinationPort; }
    uint32_t SequenceNumber() const { return m_sequenceNumber; }
    uint32_t AcknowledgmentNumber() const { return m_acknowledgmentNumber; }
    uint8_t DataOffsetByteCount() const override { return m_dataOffsetByteCount; }
    uint16_t WindowSize() const { return m_windowSize; }
    const std::vector<std::pair<OptionKinds, std::vector<uint8_t>>>& OptionList() const { return m_optionList; }
    Flags FlagBits() const { return m_flags; }
    uint8_t FlagsRaw() const override { return m_flags.flagData; }
    uint16_t Checksum() const override { return m_checksum; }
    RFC1700Protocol TransportProtocol() const override { return RFC1700Protocol::TCP; }

    int PayloadDataLength() const { return PacketEndIndex() - PacketStartIndex() - m_dataOffsetByteCount + 1; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

private:
    std::vector<std::pair<OptionKinds, std::vector<uint8_t>>> GetOptionList(int optionStartIndex);
};

} // namespace Packets
} // namespace PacketParser
