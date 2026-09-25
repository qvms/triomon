#include "SatoriDhcpOsFingerprinter.hpp"
#include <QDebug>
#include <sstream>

namespace PacketHandlerFramework {
namespace Fingerprints {

SatoriDhcpOsFingerprinter::SatoriDhcpOsFingerprinter(const QString& fingerprintFile) {
    QFile file(fingerprintFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open DHCP fingerprint file:" << fingerprintFile;
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("fingerprint")) {
                signatureList.append(Signature(xml));
            }
        }
    }
}

QStringList SatoriDhcpOsFingerprinter::GetOperatingSystems(const QList<PacketParser::Packets::AbstractPacket*>& packets) const {
    QStringList osList;
    TryGetOperatingSystems(osList, packets);
    return osList;
}

bool SatoriDhcpOsFingerprinter::TryGetOperatingSystems(QStringList& osList, const QList<PacketParser::Packets::AbstractPacket*>& packets) const {
    PacketParser::Packets::DhcpPacket* dhcpPacket = nullptr;
    PacketParser::Packets::IPv4Packet* ipPacket = nullptr;

    for (PacketParser::Packets::AbstractPacket* packet : packets) {
        if (auto* dhcp = dynamic_cast<PacketParser::Packets::DhcpPacket*>(packet)) {
            dhcpPacket = dhcp;
        } else if (auto* ipv4 = dynamic_cast<PacketParser::Packets::IPv4Packet*>(packet)) {
            ipPacket = ipv4;
        }
    }

    if (dhcpPacket == nullptr) {
        return false;
    }

    bool matched = false;
    for (const Signature& sig : signatureList) {
        if (sig.Matches(dhcpPacket, ipPacket)) {
            osList.append(sig.GetOsName());
            matched = true;
        }
    }

    return matched;
}

SatoriDhcpOsFingerprinter::Signature::Signature(QXmlStreamReader& xmlReader) {
    QXmlStreamAttributes attributes = xmlReader.attributes();
    osName = attributes.value("os").toString();
    threshold = attributes.value("threshold").toInt();

    while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == QLatin1String("fingerprint"))) {
        if (xmlReader.tokenType() == QXmlStreamReader::StartElement && xmlReader.name() == QLatin1String("test")) {
            testList.append(Test(xmlReader));
        }
        xmlReader.readNext();
    }
}

bool SatoriDhcpOsFingerprinter::Signature::Matches(PacketParser::Packets::DhcpPacket* dhcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) const {
    int currentScore = 0;
    for (const Test& test : testList) {
        if (test.Matches(dhcpPacket, ipPacket)) {
            currentScore += test.GetWeight();
        }
    }
    return currentScore >= threshold;
}

SatoriDhcpOsFingerprinter::Signature::Test::Test(QXmlStreamReader& xmlReader) {
    QXmlStreamAttributes attributes = xmlReader.attributes();
    for (const QXmlStreamAttribute& attr : attributes) {
        attributeList[attr.name().toString()] = attr.value().toString();
        if (attr.name() == QLatin1String("weight")) {
            weight = attr.value().toInt();
        }
    }
}

bool SatoriDhcpOsFingerprinter::Signature::Test::Matches(PacketParser::Packets::DhcpPacket* dhcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) const {
    for (auto it = attributeList.begin(); it != attributeList.end(); ++it) {
        QString key = it->first;
        QString value = it->second;

        if (key == "weight" || key == "matchtype") {
            continue;
        } else if (key == "dhcptype") {
            if (dhcpPacket->GetDhcpMessageType() == 1 && value != "Discover") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 2 && value != "Offer") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 3 && value != "Request") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 4 && value != "Decline") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 5 && value != "ACK") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 6 && value != "NAK") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 7 && value != "Release") return false;
            else if (dhcpPacket->GetDhcpMessageType() == 8 && value != "Inform") return false;
        } else if (key == "dhcpoptions") {
            QStringList options;
            for (const auto& opt : dhcpPacket->GetOptionList()) {
                options.append(QString::number(opt.OptionCode));
            }
            if (options.isEmpty() || options.join(",") != value) return false;
        } else if (key == "dhcpvendorcode") {
            bool found = false;
            for (const auto& opt : dhcpPacket->GetOptionList()) {
                if (opt.OptionCode == 60) {
                    if (QString::fromLatin1(opt.OptionValue) != value) return false;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        } else if (key == "dhcpttl") {
            if (ipPacket == nullptr || QString::number(ipPacket->GetTimeToLive()) != value) return false;
        } else if (key == "dhcpoption51") {
            bool found = false;
            for (const auto& opt : dhcpPacket->GetOptionList()) {
                if (opt.OptionCode == 51) {
                    uint32_t optVal = 0;
                    if (opt.OptionValue.size() >= 4) {
                        optVal = (static_cast<uint8_t>(opt.OptionValue[0]) << 24) |
                                 (static_cast<uint8_t>(opt.OptionValue[1]) << 16) |
                                 (static_cast<uint8_t>(opt.OptionValue[2]) << 8) |
                                 (static_cast<uint8_t>(opt.OptionValue[3]));
                    }
                    if (value == "infinite") {
                        if (optVal != 0xFFFFFFFF) return false;
                    } else {
                        if (QString::number(optVal) != value) return false;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        } else if (key == "dhcpoption55") {
            bool found = false;
            for (const auto& opt : dhcpPacket->GetOptionList()) {
                if (opt.OptionCode == 55) {
                    QStringList bytes;
                    for (char b : opt.OptionValue) {
                        bytes.append(QString::number(static_cast<uint8_t>(b)));
                    }
                    if (bytes.join(",") != value) return false;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        } else if (key == "dhcpoption57") {
            bool found = false;
            for (const auto& opt : dhcpPacket->GetOptionList()) {
                if (opt.OptionCode == 57) {
                    uint16_t optVal = 0;
                    if (opt.OptionValue.size() >= 2) {
                        optVal = (static_cast<uint8_t>(opt.OptionValue[0]) << 8) |
                                 (static_cast<uint8_t>(opt.OptionValue[1]));
                    }
                    if (QString::number(optVal) != value) return false;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    }
    return true;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
