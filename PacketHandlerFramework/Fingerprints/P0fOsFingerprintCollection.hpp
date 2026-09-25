#pragma once

#include "AbstractTtlDistanceCalculator.hpp"
#include "IOsFingerprinter.hpp"
#include "../../PacketParser/Fingerprints/ITtlDistanceCalculator.hpp"
#include "../../PacketParser/Packets/IPv4Packet.hpp"
#include "../../PacketParser/Packets/TcpPacket.hpp"

#include <vector>
#include <string>
#include <memory>
#include <QList>
#include <QString>

namespace PacketHandlerFramework {
namespace Fingerprints {

class P0fOsFingerprintCollection : public AbstractTtlDistanceCalculator, public IOsFingerprinter, public PacketParser::Fingerprints::ITtlDistanceCalculator {
public:
    class P0fFingerprint {
    private:
        QString windowSize;
        uint8_t initialTtl;
        bool dontFragment;
        QString overallSynPacketSize;
        QString optionValue;
        QString quirksList;
        QString osGenre;
        QString osDetails;

    public:
        explicit P0fFingerprint(const QString& fingerprintString);

        QString OsGenre() const { return osGenre; }
        QString OsDetails() const { return osDetails; }
        QString OS() const { return osGenre + " " + osDetails; }
        uint8_t InitialTTL() const { return initialTtl; }

        bool Matches(PacketParser::Packets::IPv4Packet* ipPacket, PacketParser::Packets::TcpPacket* tcpPacket, uint8_t originalTimeToLive) const;

        QString ToString() const { return "P0f"; }
    };

private:
    std::vector<std::shared_ptr<P0fFingerprint>> synOsFingerprints;
    std::vector<std::shared_ptr<P0fFingerprint>> synAckOsFingerprints;
    int maxTtlDistance;
    bool timeToLiveExists[256] = { false };

    std::vector<std::shared_ptr<P0fFingerprint>> GetFingerprintList(const QString& fingerprintFile);
    uint8_t GetOriginalTimeToLive(PacketParser::Packets::IPv4Packet* ipv4Packet, PacketParser::Packets::TcpPacket* tcpPacket);

public:
    P0fOsFingerprintCollection(const QString& synFingerprintFile, const QString& synAckFingerprintFile);

    // IOsFingerprinter Members
    bool TryGetOperatingSystems(QList<DeviceFingerprint>& osList, const QList<PacketParser::Packets::AbstractPacket*>& packetList) override;
    
    // AbstractTtlDistanceCalculator overrides (ITtlDistanceCalculator equivalent methods)
    bool TryGetTtlDistance(uint8_t& ttlDistance, const std::vector<void*>& packetList) override;
    uint8_t GetTtlDistance(uint8_t ipTimeToLive) override;
    
    uint8_t GetTtlDistance(PacketParser::Packets::IPv4Packet* ipv4Packet, PacketParser::Packets::TcpPacket* tcpPacket);
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
