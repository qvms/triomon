#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include <QByteArray>
#include <QFile>
#include <QXmlStreamReader>
#include <memory>
#include <map>
#include "PacketParser/Fingerprints/IOsFingerprinter.hpp"
#include "PacketParser/Packets/AbstractPacket.hpp"
#include "PacketParser/Packets/DhcpPacket.hpp"
#include "PacketParser/Packets/IPv4Packet.hpp"

namespace PacketHandlerFramework {
namespace Fingerprints {

class SatoriDhcpOsFingerprinter : public PacketParser::Fingerprints::IOsFingerprinter {
private:
    class Signature {
    private:
        class Test {
        private:
            std::map<QString, QString> attributeList;
            int weight;

        public:
            Test(QXmlStreamReader& xmlReader);
            bool Matches(PacketParser::Packets::DhcpPacket* dhcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) const;
            int GetWeight() const { return weight; }
        };

        QString osName;
        int threshold;
        QList<Test> testList;

    public:
        Signature(QXmlStreamReader& xmlReader);
        QString GetOsName() const { return osName; }
        bool Matches(PacketParser::Packets::DhcpPacket* dhcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) const;
    };

    QList<Signature> signatureList;

public:
    SatoriDhcpOsFingerprinter(const QString& fingerprintFile);

    QString GetName() const override { return "Satori DHCP"; }
    QStringList GetOperatingSystems(const QList<PacketParser::Packets::AbstractPacket*>& packets) const override;
    bool TryGetOperatingSystems(QStringList& osList, const QList<PacketParser::Packets::AbstractPacket*>& packets) const override;
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
