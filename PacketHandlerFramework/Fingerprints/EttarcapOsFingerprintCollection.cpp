#include "EttarcapOsFingerprintCollection.hpp"
#include <QFile>
#include <QTextStream>
#include <QString>

namespace PacketHandlerFramework {
namespace Fingerprints {

EttarcapOsFingerprintCollection::EttarcapOsFingerprintCollection(const std::string& osFingerprintFilename) {
    maxTtlDistance = 31;
    for (int i = 0; i < 256; ++i) {
        timeToLiveExists[i] = false;
    }

    QFile file(QString::fromStdString(osFingerprintFilename));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.length() > 0 && !line.startsWith("#")) {
                QString osKey;
                QString vendor;
                if (line.length() > 29) {
                    osKey = line.mid(0, 28);
                    vendor = line.mid(29);
                }
                if (!osKey.isEmpty() && !vendor.isEmpty()) {
                    osDictionary[osKey.toStdString()].push_back(vendor.toStdString());
                    
                    bool ok;
                    int ttl = osKey.mid(10, 2).toInt(&ok, 16);
                    if (ok && ttl >= 0 && ttl < 256) {
                        timeToLiveExists[ttl] = true;
                    }
                }
            }
        }
    }
}

std::vector<std::string> EttarcapOsFingerprintCollection::GetOperatingSystems(void* ipv4Packet, void* tcpPacket, uint8_t originalTimeToLive) {
    // Stub
    return {};
}

std::string EttarcapOsFingerprintCollection::GetEttercapOperatingSystemFingerprint(void* ipv4Packet, void* tcpPacket, uint8_t originalTimeToLive) {
    // Stub
    return "";
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
