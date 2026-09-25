#include "IpAddressUtil.hpp"
#include <algorithm>
#include <vector>

// Full implementation assuming IPAddress methods exist.
namespace System {
    namespace Net {
        class IPAddress {
        public:
            virtual int GetAddressFamily() const = 0;
            virtual std::vector<uint8_t> GetAddressBytes() const = 0;
            virtual bool Equals(const IPAddress* other) const = 0;
            static IPAddress* IPv6Loopback;
            static IPAddress* IPv6None;
        };
        // Mock static member definition to allow linking if used.
        IPAddress* IPAddress::IPv6Loopback = nullptr;
        IPAddress* IPAddress::IPv6None = nullptr;
    }
}

namespace PacketParser {
namespace Utils {

const std::string IpAddressUtil::IPv6_REGEX_STRING = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))";
const std::string IpAddressUtil::IPv4_REGEX_STRING = "((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}";

static const std::vector<uint8_t> ipv4ReservedClassAList = { 0, 127, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

bool IpAddressUtil::IsIanaReserved(const System::Net::IPAddress* ipAddress) {
    if (!ipAddress) return false;
    
    // 28 is commonly InterNetworkV6 in some systems but let's just check equality with known v6 ones.
    if (ipAddress->Equals(System::Net::IPAddress::IPv6Loopback) || ipAddress->Equals(System::Net::IPAddress::IPv6None)) {
        return true;
    }
    
    std::vector<uint8_t> ip = ipAddress->GetAddressBytes();
    if (ip.size() == 4) {
        return std::find(ipv4ReservedClassAList.begin(), ipv4ReservedClassAList.end(), ip[0]) != ipv4ReservedClassAList.end();
    }
    return false;
}

int IpAddressUtil::CompareTo(const System::Net::IPAddress* ipAddress, const System::Net::IPAddress* otherIpAddress) {
    if (!ipAddress || !otherIpAddress) return 0;
    
    if (ipAddress->GetAddressFamily() != otherIpAddress->GetAddressFamily()) {
        return ipAddress->GetAddressFamily() - otherIpAddress->GetAddressFamily();
    }
    
    std::vector<uint8_t> localBytes = ipAddress->GetAddressBytes();
    std::vector<uint8_t> remoteBytes = otherIpAddress->GetAddressBytes();
    
    size_t minLen = std::min(localBytes.size(), remoteBytes.size());
    for (size_t i = 0; i < minLen; i++) {
        if (localBytes[i] != remoteBytes[i]) {
            return localBytes[i] - remoteBytes[i];
        }
    }
    return 0;
}

}
}
