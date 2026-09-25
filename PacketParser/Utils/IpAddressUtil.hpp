#pragma once
#include <vector>
#include <string>

namespace System {
    namespace Net {
        class IPAddress; // forward declaration
    }
}

namespace PacketParser {
namespace Utils {

class IpAddressUtil {
public:
    static const std::string IPv6_REGEX_STRING;
    static const std::string IPv4_REGEX_STRING;

    static bool IsIanaReserved(const System::Net::IPAddress* ipAddress);
    static int CompareTo(const System::Net::IPAddress* ipAddress, const System::Net::IPAddress* otherIpAddress);
};

}
}