#include "DeviceFingerprint.hpp"

namespace PacketHandlerFramework {
namespace Fingerprints {

DeviceFingerprint::DeviceFingerprint(const std::string& os, const std::string& category, const std::string& family) 
    : os(os), category(category), family(family) {
}

std::string DeviceFingerprint::ToString() const {
    std::string osString = this->os;

    if (!this->category.empty()) {
        osString += " [" + this->category + "]";
    }
    if (!this->family.empty()) {
        osString += " [" + this->family + "]";
    }
    return osString;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
