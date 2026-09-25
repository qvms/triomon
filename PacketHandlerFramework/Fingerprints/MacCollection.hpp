#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <memory>

namespace PacketHandlerFramework {
namespace Fingerprints {

class MacCollection {
public:
    enum class MacFingerprintFileFormat { Ettercap, Nmap, IEEE_OUI, IEEE_OUI36 };

private:
    static std::shared_ptr<MacCollection> singletonInstance;
    
    std::map<long long, std::string> mac48Dictionary;
    std::map<long long, std::string> mac36Dictinoary;
    std::map<long long, std::string> mac24Dictionary;

    struct DataSource {
        std::string macFingerprintFilename;
        MacFingerprintFileFormat format;
    };

    MacCollection(const std::vector<DataSource>& sources);

public:
    static std::shared_ptr<MacCollection> GetSingletonInstance();
    static void DeleteSingleton();

    std::string GetMacVendor(const std::string& macAddress);
    bool TryGetMacVendor(const std::vector<uint8_t>& macAddress, std::string& macVendor);
    bool TryGetMacVendor(const std::string& macAddress, std::string& macVendor);
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
