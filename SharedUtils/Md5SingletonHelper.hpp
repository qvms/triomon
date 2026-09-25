#pragma once

#include <vector>
#include <string>

namespace SharedUtils {

class Md5SingletonHelper {
public:
    static std::string GetMd5HashString(const std::vector<uint8_t>& data);
    static std::string GetMd5HashString(const std::vector<uint8_t>& data, int offset, int count);
};

}