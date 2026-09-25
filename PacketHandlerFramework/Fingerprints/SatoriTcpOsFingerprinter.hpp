#pragma once

#include "PacketHandlerFramework/Fingerprints/IOsFingerprinterInfo.hpp"
#include "PacketHandlerFramework/Fingerprints/IOsFingerprinter.hpp"
#include "PacketHandlerFramework/Fingerprints/AbstractTtlDistanceCalculator.hpp"
#include "PacketHandlerFramework/Fingerprints/P0fOsFingerprintCollection.hpp"
#include "Packets/TcpPacket.hpp"
#include "Packets/IPv4Packet.hpp"
#include <QString>
#include <QList>
#include <QSet>
#include <vector>
#include <memory>

class QDomNode; // Or appropriate Qt XML equivalent if needed, but we stub it

namespace PacketHandlerFramework {
namespace Fingerprints {

class SatoriTcpOsFingerprinter : public IOsFingerprinter, public IOsFingerprinterInfo {
private:
    class TcpFingerprint {
    private:
        class Test : public AbstractTtlDistanceCalculator {
        private:
            int weight;
            QSet<QChar> tcpflags;
            std::unique_ptr<P0fOsFingerprintCollection::P0fFingerprint> p0fFingerprint;

        public:
            Test(const QString& weightStr, const QString& tcpSig, const QString& tcpFlag, const QString& osClass, const QString& osDetails);
            int Weight() const { return weight; }
            bool TcpFlagsMatch(PacketParser::Packets::TcpPacket* tcpPacket) const;
            bool Matches(PacketParser::Packets::TcpPacket* tcpPacket, PacketParser::Packets::IPv4Packet* ipPacket);
        };

        QString os;
        QString osClass;
        QList<std::shared_ptr<Test>> testList;
        std::vector<QList<std::shared_ptr<Test>>> tcpFlagsTestLists;

    public:
        TcpFingerprint(const QString& os, const QString& osClass);
        QString ToString() const;
        void AddTest(const QString& weightStr, const QString& tcpSig, const QString& tcpFlag);
        bool ContainsFingerprintsForTcpFlags(PacketParser::Packets::TcpPacket* tcpPacket);
        int GetHighestMatchWeight(PacketParser::Packets::TcpPacket* tcpPacket, PacketParser::Packets::IPv4Packet* ipPacket);
    };

    QList<std::shared_ptr<TcpFingerprint>> fingerprintList;
    std::vector<QList<std::shared_ptr<TcpFingerprint>>> tcpFlagsFingerprintList;
    double confidence;

public:
    SatoriTcpOsFingerprinter(const QString& satoriTcpXmlFilename, double confidence = 0.4);
    ~SatoriTcpOsFingerprinter() override = default;

    double Confidence() const override { return confidence; }
    QString Name() const override { return "Satori TCP"; }
    
    int CompareTo(IOsFingerprinter* other) const { return Name().compare(other->Name()); }
    int CompareTo(IOsFingerprinterInfo* other) const { return Name().compare(other->Name()); }

    bool TryGetOperatingSystems(QList<DeviceFingerprint>& osList, const QList<PacketParser::Packets::AbstractPacket*>& packetList) override;
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
