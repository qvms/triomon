#include "MacCollection.hpp"
#include "DictionaryFactory.hpp"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <iomanip>
#include <sstream>

namespace PacketHandlerFramework {
namespace Fingerprints {

std::shared_ptr<MacCollection> MacCollection::singletonInstance = nullptr;

std::shared_ptr<MacCollection> MacCollection::GetSingletonInstance() {
    if (!singletonInstance) {
        std::string fingerprintPath = "osDetect/";
        std::vector<DataSource> databases = {
            { fingerprintPath + "oui.txt", MacFingerprintFileFormat::IEEE_OUI },
            { fingerprintPath + "oui36.csv", MacFingerprintFileFormat::IEEE_OUI36 }
        };
        singletonInstance = std::shared_ptr<MacCollection>(new MacCollection(databases));
    }
    return singletonInstance;
}

void MacCollection::DeleteSingleton() {
    singletonInstance = nullptr;
}

MacCollection::MacCollection(const std::vector<DataSource>& sources) {
    mac48Dictionary[0xffffffffffff] = "IEEE Broadcast";
    
    for (long long i = 0; i < 0x800; i++) {
        mac36Dictinoary[0x01005e000 + i] = "IEEE Multicast";
    }
    
    for (const auto& source : sources) {
        if (source.format == MacFingerprintFileFormat::IEEE_OUI || source.format == MacFingerprintFileFormat::Ettercap || source.format == MacFingerprintFileFormat::Nmap) {
            QFile file(QString::fromStdString(source.macFingerprintFilename));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    if (!line.isEmpty() && !line.startsWith("#")) {
                        QString macKey;
                        QString vendor;
                        
                        if (source.format == MacFingerprintFileFormat::Ettercap && line.length() > 10) {
                            macKey = line.mid(0, 8);
                            vendor = line.mid(10);
                        } else if (source.format == MacFingerprintFileFormat::Nmap && line.length() > 7) {
                            macKey = line.mid(0, 2) + ":" + line.mid(2, 2) + ":" + line.mid(4, 2);
                            vendor = line.mid(7);
                        } else if (source.format == MacFingerprintFileFormat::IEEE_OUI && line.length() > 15 && line.contains("(hex)") && line.trimmed().at(2) == '-') {
                            line = line.trimmed();
                            macKey = line.mid(0, 8).replace('-', ':');
                            vendor = line.mid(line.lastIndexOf('\t') + 1);
                        }
                        
                        if (!macKey.isEmpty() && !vendor.isEmpty()) {
                            bool ok;
                            long long m = macKey.replace(":", "").toLongLong(&ok, 16);
                            if (ok) {
                                if (mac24Dictionary.find(m) == mac24Dictionary.end()) {
                                    mac24Dictionary[m] = vendor.toStdString();
                                }
                            }
                        }
                    }
                }
            }
        } else if (source.format == MacFingerprintFileFormat::IEEE_OUI36) {
            auto macDict = DictionaryFactory::CreateDictionaryFromCsv(source.macFingerprintFilename, 1, 2, true);
            for (const auto& kvp : macDict) {
                QString keyStr = QString::fromStdString(kvp.first).trimmed();
                bool ok;
                long long m = keyStr.toLongLong(&ok, 16);
                if (ok) {
                    if (mac36Dictinoary.find(m) == mac36Dictinoary.end()) {
                        QString orgName = QString::fromStdString(kvp.second).trimmed();
                        orgName.remove('"');
                        if (!orgName.isEmpty()) {
                            mac36Dictinoary[m] = orgName.toStdString();
                        }
                    }
                }
            }
        }
    }
}

std::string MacCollection::GetMacVendor(const std::string& macAddress) {
    std::string vendor;
    if (TryGetMacVendor(macAddress, vendor)) {
        return vendor;
    }
    return "Unknown";
}

bool MacCollection::TryGetMacVendor(const std::vector<uint8_t>& macAddress, std::string& macVendor) {
    std::ostringstream macWithColons;
    macWithColons << std::hex << std::setfill('0');
    for (size_t i = 0; i < macAddress.size(); ++i) {
        macWithColons << std::setw(2) << static_cast<int>(macAddress[i]);
        if (i < macAddress.size() - 1) {
            macWithColons << ":";
        }
    }
    return TryGetMacVendor(macWithColons.str(), macVendor);
}

bool MacCollection::TryGetMacVendor(const std::string& macAddress, std::string& macVendor) {
    QString addrStr = QString::fromStdString(macAddress).replace(":", "");
    bool ok;
    long long mac48 = addrStr.toLongLong(&ok, 16);
    
    if (ok) {
        if (mac48 >= 0 && mac48 < 281474976710656LL) {
            if (mac48Dictionary.find(mac48) != mac48Dictionary.end()) {
                macVendor = mac48Dictionary[mac48];
                return true;
            } else if (mac36Dictinoary.find(mac48 >> 12) != mac36Dictinoary.end()) {
                macVendor = mac36Dictinoary[mac48 >> 12];
                return true;
            } else if (mac24Dictionary.find(mac48 >> 24) != mac24Dictionary.end()) {
                macVendor = mac24Dictionary[mac48 >> 24];
                return true;
            }
        }
    }
    return false;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
