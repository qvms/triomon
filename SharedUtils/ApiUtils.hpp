#pragma once
#include <string>

namespace SharedUtils {

class ApiUtils {
public:
    // Returns a parsed version string if successful
    static std::string GetLatestVersion(const std::string& productCode, std::string& releasePost, std::string& downloadUrl);
};

}