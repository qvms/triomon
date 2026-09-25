#pragma once
#include "AbstractPacket.hpp"
#include <QHostAddress>
#include <vector>

namespace PacketParser {
namespace Packets {

class ArpPacket : public AbstractPacket {
private:
    uint16_t m_hardwareType;
    uint16_t m_operation;
    uint16_t m_protocolType;
    uint8_t m_hardwareLength;
    uint8_t m_protocolLength;
    std::vector<uint8_t> m_senderHardwareAddress;
    std::vector<uint8_t> m_senderProtocolAddress;
    std::vector<uint8_t> m_targetHardwareAddress;
    std::vector<uint8_t> m_targetProtocolAddress;

public:
    ArpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<uint8_t> SenderHardwareAddress() const { return m_senderHardwareAddress; }
    std::vector<uint8_t> TargetHardwareAddress() const { return m_targetHardwareAddress; }
    
    QHostAddress SenderIPAddress() const;
    QHostAddress TargetIPAddress() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
