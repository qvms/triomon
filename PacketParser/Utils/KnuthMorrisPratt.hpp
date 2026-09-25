#pragma once

#include <vector>
#include <cstdint>
#include <istream>

namespace PacketParser {
namespace Utils {

class KnuthMorrisPratt {
public:
    static long long ReadTo(const std::vector<uint8_t>& pattern, const std::vector<uint8_t>& data, int offset, std::vector<uint8_t>& readBytes);
    static long long ReadTo(const std::vector<uint8_t>& pattern, const std::vector<uint8_t>& data, int offset, std::vector<uint8_t>& readBytes, const std::vector<int>& kmpFailureFunction);
    
    static long long ReadTo(const std::vector<uint8_t>& pattern, std::istream& stream, std::vector<uint8_t>& readBytes);
    static long long ReadTo(const std::vector<uint8_t>& pattern, std::istream& stream, std::vector<uint8_t>& readBytes, const std::vector<int>& kmpFailureFunction);
    
    static std::vector<int> KmpFailureFunction(const std::vector<uint8_t>& pattern);
};

} // namespace Utils
} // namespace PacketParser