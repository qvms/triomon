#pragma once
#include <vector>
#include <cstdint>

namespace PacketParser {
namespace Utils {

class BoyerMoore {
public:
    static int IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, int offset = 0);
    static int LastIndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, int offset = 0);

    static int IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, const std::vector<int>& byteTable, const std::vector<int>& offsetTable, int offset = 0);
    static int IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, const std::vector<int>& byteTable, const std::vector<int>& offsetTable, bool ignoreCase, int offset = 0);

    static std::vector<uint8_t> ToUpper(const std::vector<uint8_t>& b);
    static uint8_t ToUpper(uint8_t b);

    static std::vector<int> MakeByteTable(const std::vector<uint8_t>& needle);
    static std::vector<int> MakeByteTable(const std::vector<uint8_t>& needle, bool ignoreCase);
    static std::vector<int> MakeOffsetTable(const std::vector<uint8_t>& needle);

private:
    static bool isPrefix(const std::vector<uint8_t>& needle, int p);
    static int suffixLength(const std::vector<uint8_t>& needle, int p);
};

} // namespace Utils
} // namespace PacketParser