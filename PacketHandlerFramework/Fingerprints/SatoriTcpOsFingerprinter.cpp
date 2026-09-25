#include "SatoriTcpOsFingerprinter.hpp"
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>

namespace PacketHandlerFramework {
namespace Fingerprints {

SatoriTcpOsFingerprinter::TcpFingerprint::Test::Test(const QString& weightStr, const QString& tcpSig, const QString& tcpFlag, const QString& osClass, const QString& osDetails) {
    weight = weightStr.toInt();
    if (!tcpSig.isEmpty()) {
        QString sig = tcpSig + ":" + osClass + ":" + osDetails;
        p0fFingerprint = std::make_unique<P0fOsFingerprintCollection::P0fFingerprint>(sig);
    }
    for (int i = 0; i < tcpFlag.length(); ++i) {
        tcpflags.insert(tcpFlag[i]);
    }
}

bool SatoriTcpOsFingerprinter::TcpFingerprint::Test::TcpFlagsMatch(PacketParser::Packets::TcpPacket* tcpPacket) const {
    if (tcpPacket->FlagBits().Synchronize != tcpflags.contains('S')) return false;
    if (tcpPacket->FlagBits().Acknowledgement != tcpflags.contains('A')) return false;
    if (tcpPacket->FlagBits().Fin != tcpflags.contains('F')) return false;
    return true;
}

bool SatoriTcpOsFingerprinter::TcpFingerprint::Test::Matches(PacketParser::Packets::TcpPacket* tcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) {
    if (!TcpFlagsMatch(tcpPacket)) return false;
    if (!p0fFingerprint) return false;
    
    if (!p0fFingerprint->Matches(ipPacket, tcpPacket, ipPacket->TimeToLive()) &&
        !p0fFingerprint->Matches(ipPacket, tcpPacket, AbstractTtlDistanceCalculator::GetOriginalTimeToLive(ipPacket->TimeToLive()))) {
        return false;
    }
    return true;
}

SatoriTcpOsFingerprinter::TcpFingerprint::TcpFingerprint(const QString& os, const QString& osClass)
    : os(os), osClass(osClass) {
    tcpFlagsTestLists.resize(256);
}

QString SatoriTcpOsFingerprinter::TcpFingerprint::ToString() const {
    if (!os.isEmpty() && !osClass.isEmpty()) return osClass + " - " + os;
    if (!os.isEmpty()) return os;
    if (!osClass.isEmpty()) return osClass;
    return "TcpFingerprint";
}

void SatoriTcpOsFingerprinter::TcpFingerprint::AddTest(const QString& weightStr, const QString& tcpSig, const QString& tcpFlag) {
    testList.append(std::make_shared<Test>(weightStr, tcpSig, tcpFlag, osClass, os));
}

bool SatoriTcpOsFingerprinter::TcpFingerprint::ContainsFingerprintsForTcpFlags(PacketParser::Packets::TcpPacket* tcpPacket) {
    uint8_t flagsRaw = tcpPacket->FlagsRaw();
    if (tcpFlagsTestLists[flagsRaw].isEmpty()) {
        for (const auto& t : testList) {
            if (t->TcpFlagsMatch(tcpPacket)) {
                tcpFlagsTestLists[flagsRaw].append(t);
            }
        }
        if (tcpFlagsTestLists[flagsRaw].isEmpty()) {
            tcpFlagsTestLists[flagsRaw].append(nullptr); // Mark as checked but empty
        }
    }
    return tcpFlagsTestLists[flagsRaw].first() != nullptr;
}

int SatoriTcpOsFingerprinter::TcpFingerprint::GetHighestMatchWeight(PacketParser::Packets::TcpPacket* tcpPacket, PacketParser::Packets::IPv4Packet* ipPacket) {
    int highestWeight = -1;
    if (ContainsFingerprintsForTcpFlags(tcpPacket)) {
        for (const auto& t : tcpFlagsTestLists[tcpPacket->FlagsRaw()]) {
            if (t && t->Weight() >= 3 && t->Weight() > highestWeight && t->Matches(tcpPacket, ipPacket)) {
                highestWeight = t->Weight();
            }
        }
    }
    return highestWeight;
}

SatoriTcpOsFingerprinter::SatoriTcpOsFingerprinter(const QString& satoriTcpXmlFilename, double confidence)
    : confidence(confidence) {
    tcpFlagsFingerprintList.resize(256);
    
    QFile file(satoriTcpXmlFilename);
    if (!file.open(QIODevice::ReadOnly)) return;

    QDomDocument tcpXml;
    if (!tcpXml.setContent(&file)) return;

    QDomElement fingerprintsNode = tcpXml.documentElement().firstChildElement();
    QDomNodeList fingerprintNodes = fingerprintsNode.elementsByTagName("fingerprint");
    
    for (int i = 0; i < fingerprintNodes.count(); ++i) {
        QDomElement el = fingerprintNodes.at(i).toElement();
        QString osClass = el.attribute("os_class", "");
        QString os = el.attribute("os_name", "");
        if (os.isEmpty()) os = el.attribute("name", "");
        
        auto fingerprint = std::make_shared<TcpFingerprint>(os, osClass);
        fingerprintList.append(fingerprint);
        
        QDomNodeList tests = el.elementsByTagName("test");
        for (int j = 0; j < tests.count(); ++j) {
            QDomElement testEl = tests.at(j).toElement();
            QString weightStr = testEl.attribute("weight");
            QString tcpSig = testEl.attribute("tcpsig");
            QString tcpFlag = testEl.attribute("tcpflag");
            fingerprint->AddTest(weightStr, tcpSig, tcpFlag);
        }
    }
}

bool SatoriTcpOsFingerprinter::TryGetOperatingSystems(QList<DeviceFingerprint>& osList, const QList<PacketParser::Packets::AbstractPacket*>& packetList) {
    PacketParser::Packets::TcpPacket* tcpPacket = nullptr;
    PacketParser::Packets::IPv4Packet* ipPacket = nullptr;

    for (PacketParser::Packets::AbstractPacket* p : packetList) {
        if (auto* tcp = dynamic_cast<PacketParser::Packets::TcpPacket*>(p)) {
            tcpPacket = tcp;
        } else if (auto* ipv4 = dynamic_cast<PacketParser::Packets::IPv4Packet*>(p)) {
            ipPacket = ipv4;
        }
    }

    if (tcpPacket && ipPacket) {
        uint8_t flagsRaw = tcpPacket->FlagsRaw();
        if (tcpFlagsFingerprintList[flagsRaw].isEmpty()) {
            for (const auto& f : fingerprintList) {
                if (f->ContainsFingerprintsForTcpFlags(tcpPacket)) {
                    tcpFlagsFingerprintList[flagsRaw].append(f);
                }
            }
            if (tcpFlagsFingerprintList[flagsRaw].isEmpty()) {
                tcpFlagsFingerprintList[flagsRaw].append(nullptr); // Mark as checked
            }
        }

        if (tcpFlagsFingerprintList[flagsRaw].first() != nullptr) {
            int osListWeight = 3;
            for (const auto& f : tcpFlagsFingerprintList[flagsRaw]) {
                if (!f) continue;
                int w = f->GetHighestMatchWeight(tcpPacket, ipPacket);
                if (w > osListWeight) {
                    osListWeight = w;
                    osList.clear();
                    osList.append(DeviceFingerprint(f->ToString()));
                } else if (w == osListWeight) {
                    osList.append(DeviceFingerprint(f->ToString()));
                }
            }
            if (!osList.isEmpty()) return true;
        }
    }
    return false;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
