#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class NtlmSspPacket : public AbstractPacket {
public:
    enum class NtlmMessageTypes : uint32_t {
        Negotiate = 0x01000000,
        Challenge = 0x02000000,
        Authentication = 0x03000000
    };

    struct SecurityBuffer {
        uint16_t length;
        uint16_t lengthAllocated;
        uint32_t offset;

        SecurityBuffer(const uint8_t* data, int& dataOffset);
        std::vector<uint8_t> GetBufferData(const uint8_t* frameData, int packetStartIndex) const;
    };

private:
    uint32_t messageType;
    QString domainName;
    QString userName;
    QString hostName;
    QString lanManagerResponse;
    QString ntlmResponse;
    QString ntlmChallenge;

    NtlmSspPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result);

    uint32_t MessageType() const { return messageType; }
    QString DomainName() const { return domainName; }
    QString UserName() const { return userName; }
    QString HostName() const { return hostName; }
    QString LanManagerResponse() const { return lanManagerResponse; }
    QString NtlmResponse() const { return ntlmResponse; }
    QString NtlmChallenge() const { return ntlmChallenge; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
