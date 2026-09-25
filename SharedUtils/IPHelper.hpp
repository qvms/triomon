#pragma once

#include <string>

namespace SharedUtils {

class IPHelper {
public:
    static bool IsPrivateIP(const std::string& ipAddress);
    static bool IsValidIP(const std::string& ipAddress);
};

}
