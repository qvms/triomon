#include "SortableIpAddress.hpp"
#include <algorithm>
#include <stdexcept>

// Mock IPAddress 
namespace System {
    namespace Net {
        class IPAddress {
        public:
            virtual int GetAddressFamily() const = 0;
            virtual std::vector<uint8_t> GetAddressBytes() const = 0;
            virtual long long get_ScopeId() const { return 0; }
        };
    }
}

namespace SharedUtils {

SortableIpAddress::SortableIpAddress(const System::Net::IPAddress* ipAddress) {
    if (ipAddress) {
        addressBytes = ipAddress->GetAddressBytes();
        scopeId = ipAddress->get_ScopeId();
        addressFamily = ipAddress->GetAddressFamily();
    } else {
        scopeId = 0;
        addressFamily = 0;
    }
}

SortableIpAddress::SortableIpAddress(long long newAddress) : scopeId(0), addressFamily(2) { // 2 = InterNetwork
    addressBytes.resize(4);
    addressBytes[0] = static_cast<uint8_t>(newAddress & 0xFF);
    addressBytes[1] = static_cast<uint8_t>((newAddress >> 8) & 0xFF);
    addressBytes[2] = static_cast<uint8_t>((newAddress >> 16) & 0xFF);
    addressBytes[3] = static_cast<uint8_t>((newAddress >> 24) & 0xFF);
}

SortableIpAddress::SortableIpAddress(const std::vector<uint8_t>& address) : addressBytes(address), scopeId(0) {
    addressFamily = address.size() == 4 ? 2 : 28; // 2=IPv4, 28=IPv6 typically
}

SortableIpAddress::SortableIpAddress(const std::vector<uint8_t>& address, long long scopeId) : addressBytes(address), scopeId(scopeId) {
    addressFamily = address.size() == 4 ? 2 : 28;
}

int SortableIpAddress::CompareTo(const System::Net::IPAddress* otherIpAddress) const {
    if (!otherIpAddress) return 1;
    if (addressFamily != otherIpAddress->GetAddressFamily()) {
        return addressFamily - otherIpAddress->GetAddressFamily();
    }
    std::vector<uint8_t> remoteBytes = otherIpAddress->GetAddressBytes();
    size_t minLen = std::min(addressBytes.size(), remoteBytes.size());
    for (size_t i = 0; i < minLen; i++) {
        if (addressBytes[i] != remoteBytes[i]) {
            return addressBytes[i] - remoteBytes[i];
        }
    }
    return 0;
}

int SortableIpAddress::CompareTo(const SortableIpAddress& other) const {
    if (addressFamily != other.addressFamily) {
        return addressFamily - other.addressFamily;
    }
    size_t minLen = std::min(addressBytes.size(), other.addressBytes.size());
    for (size_t i = 0; i < minLen; i++) {
        if (addressBytes[i] != other.addressBytes[i]) {
            return addressBytes[i] - other.addressBytes[i];
        }
    }
    return 0;
}

}
