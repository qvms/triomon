#pragma once
#include "AbstractPacket.hpp"
// #include "../PayloadProtocol.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class CotpPacket : public AbstractPacket {
public:
    enum class Tpdu : uint8_t {
        ConnectionRequest = 0xe,
        ConnectionConfirm = 0xd,
        DisconnectRequest = 0x8,
        DisconnectConfirm = 0xc,
        Data = 0xf,
        ExpeditedData = 0x1,
        DataAcknowledge = 0x6,
        ExpeditedAcknowledge = 0x2,
        Reject = 0x5,
        Error = 0x7,
        UNKNOWN = 0xff
    };

    enum class ParameterCode : uint8_t {
        ACK_TIME = 0x85,
        RES_ERROR = 0x86,
        PRIORITY = 0x87,
        TRANSIT_DEL = 0x88,
        THROUGHPUT = 0x89,
        SEQ_NR = 0x8a,
        REASSIGNMENT = 0x8b,
        FLOW_CNTL = 0x8c,
        TPDU_SIZE = 0xc0,
        SRC_TSAP = 0xc1,
        DST_TSAP = 0xc2,
        CHECKSUM = 0xc3,
        VERSION_NR = 0xc4,
        PROTECTION = 0xc5,
        OPT_SEL = 0xc6,
        PROTO_CLASS = 0xc7,
        CLEARING_INFO = 0xe0,
        PREF_MAX_TPDU_SIZE = 0xf0,
        INACTIVITY_TIMER = 0xf2,
        ATN_EC_32 = 0x08,
        ATN_EC_16 = 0x09,
    };

private:
    uint8_t m_length;
    uint8_t m_pduType;
    uint8_t m_tpduNumber;
    uint8_t m_pduClass;
    bool m_endOfTsdu;

public:
    int EncapsulatedProtocol;

    CotpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, int encapsulatedProtocol);

    bool EndOfTsdu() const { return m_endOfTsdu; }
    Tpdu GetTpdu() const;

    bool TryGetTsduData(std::vector<uint8_t>& data) const;
    bool TryGetVariablePartIndex(int& variablePartIndex) const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
