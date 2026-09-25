#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class SpotifyKeyExchangePacket : public AbstractPacket, public virtual ISessionPacket {
public:
    static const uint16_t CONTENT_END_USHORT = 0x0100; // actually 0x0001 in C#, but in C# ToUInt16 uses bit converter. Wait, C# says `CONTENT_END_USHORT = 0x0001` or something? Ah, "CONTENT_END_USHORT" is just 0x0001.

private:
    uint8_t m_version;
    uint16_t m_keyExchangePacketLength;
    uint8_t m_clientOS;
    uint32_t m_clientID;
    uint32_t m_clientRevision;
    std::vector<uint8_t> m_random;
    std::vector<uint8_t> m_publicKey;
    std::vector<uint8_t> m_blob;
    std::vector<uint8_t> m_salt;
    QString m_username;
    bool m_fromClient;

    SpotifyKeyExchangePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool isFromClient);

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool fromClient, AbstractPacket** result);

    uint8_t Version() const { return m_version; }
    uint16_t KeyExchangePacketLength() const { return m_keyExchangePacketLength; }
    QString ClientOperatingSystem() const;
    uint32_t ClientID() const { return m_clientID; }
    uint32_t ClientRevision() const { return m_clientRevision; }
    const std::vector<uint8_t>& Random() const { return m_random; }
    const std::vector<uint8_t>& PublicKey() const { return m_publicKey; }
    const std::vector<uint8_t>& Blob() const { return m_blob; }
    const std::vector<uint8_t>& Salt() const { return m_salt; }
    QString Username() const { return m_username; }

    bool PacketHeaderIsComplete() const override { return true; }
    int ParsedBytesCount() const override { return PacketLength(); }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
