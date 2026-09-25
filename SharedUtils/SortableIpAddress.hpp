#pragma once
#include <vector>
#include <cstdint>

namespace System {
    namespace Net {
        class IPAddress; // forward declaration
    }
}

namespace SharedUtils {

class SortableIpAddress {
private:
    std::vector<uint8_t> addressBytes;
    long long scopeId;
    int addressFamily;

public:
    explicit SortableIpAddress(const System::Net::IPAddress* ipAddress);
    explicit SortableIpAddress(long long newAddress);
    explicit SortableIpAddress(const std::vector<uint8_t>& address);
    SortableIpAddress(const std::vector<uint8_t>& address, long long scopeId);

    int CompareTo(const System::Net::IPAddress* otherIpAddress) const;
    int CompareTo(const SortableIpAddress& other) const;
    
    std::vector<uint8_t> GetAddressBytes() const { return addressBytes; }
    int GetAddressFamily() const { return addressFamily; }
};

}
