#pragma once

#include "AbstractTtlDistanceCalculator.hpp"
#include <map>
#include <vector>
#include <string>

namespace PacketHandlerFramework {
namespace Fingerprints {

class EttarcapOsFingerprintCollection : public AbstractTtlDistanceCalculator { // also conceptually IOsFingerprinter
private:
    std::map<std::string, std::vector<std::string>> osDictionary;
    int maxTtlDistance;
    bool timeToLiveExists[256];

public:
    EttarcapOsFingerprintCollection(const std::string& osFingerprintFilename);
    virtual ~EttarcapOsFingerprintCollection() = default;

    std::string Name() const { return "Ettercap"; }
    double Confidence() const { return 0.2; }

    std::vector<std::string> GetOperatingSystems(void* ipv4Packet, void* tcpPacket, uint8_t originalTimeToLive);
    std::string GetEttercapOperatingSystemFingerprint(void* ipv4Packet, void* tcpPacket, uint8_t originalTimeToLive);
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
