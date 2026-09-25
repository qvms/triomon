#pragma once
#include "AbstractPacket.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

class HpSwitchProtocolPacket : public AbstractPacket {
private:
    uint8_t m_version;
    uint8_t m_type;

public:
    HpSwitchProtocolPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    class HpSwField : public AbstractPacket {
    public:
        enum class FieldType : uint8_t { DeviceName=0x01, Version=0x02, Config=0x03, IpAddress=0x05, MacAddress=0x0e };

    private:
        uint8_t m_typeByte;
        uint8_t m_valueLength;
        std::vector<uint8_t> m_valueBytes;

    public:
        HpSwField(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

        uint8_t TypeByte() const { return m_typeByte; }
        const std::vector<uint8_t>& ValueBytes() const { return m_valueBytes; }
        QString ValueString() const;

        std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
    };
};

} // namespace Packets
} // namespace PacketParser
