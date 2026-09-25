#pragma once
#include "AbstractPacket.hpp"
#include <vector>
#include <cstdint>

namespace PacketParser {
namespace Packets {

class TlsRecordPacket : public AbstractPacket {
public:
    enum class ContentTypes : uint8_t {
        ChangeCipherSpec = 0x14,
        Alert = 0x15,
        Handshake = 0x16,
        Application = 0x17,
    };

private:
    ContentTypes contentType;
    uint8_t versionMajor; // MSB
    uint8_t versionMinor; // LSB
    uint16_t length; // MSB & LSB

public:
    TlsRecordPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    ~TlsRecordPacket() override = default;

    bool TlsRecordIsComplete() const { return PacketEndIndex() - PacketStartIndex() + 1 == 5 + length; }
    uint16_t Length() const { return length; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    class HandshakePacket : public AbstractPacket {
    public:
        enum class MessageTypes : uint8_t {
            HelloRequest = 0x00,
            ClientHello = 0x01,
            ServerHello = 0x02,
            Certificate = 0x0b,

            ServerKeyExchange = 0x0c,
            CertificateRequest = 0x0d,
            ServerHelloDone = 0x0e,
            CertificateVerify = 0x0f,

            ClientKeyExchange = 0x10,
            Finished = 0x14,
        };

    private:
        MessageTypes messageType;
        uint32_t messageLength; // actually a 3-byte (uint24) long field
        std::vector<std::vector<uint8_t>> certificateList; // only for messageType=0x0b

    public:
        HandshakePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
        ~HandshakePacket() override = default;

        MessageTypes MessageType() const { return messageType; }
        uint32_t MessageLength() const { return messageLength; }
        const std::vector<std::vector<uint8_t>>& CertificateList() const { return certificateList; }

        std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
    };
};

} // namespace Packets
} // namespace PacketParser
