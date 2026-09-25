#pragma once

#include <string>

namespace PacketHandlerFramework {
namespace Fingerprints {

class DeviceFingerprint {
private:
    std::string os;
    std::string category;
    std::string family;

public:
    DeviceFingerprint(const std::string& os, const std::string& category = "", const std::string& family = "");

    std::string OS() const { return os; }
    std::string Category() const { return category; }
    std::string Family() const { return family; }

    std::string ToString() const;
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
