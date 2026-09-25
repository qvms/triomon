#include "BoyerMoore.hpp"
#include <algorithm>

namespace PacketParser {
namespace Utils {

int BoyerMoore::IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, int offset) {
    if (needle.empty()) return 0;
    std::vector<int> byteTable = MakeByteTable(needle);
    std::vector<int> offsetTable = MakeOffsetTable(needle);
    return IndexOf(haystack, needle, byteTable, offsetTable, offset);
}

int BoyerMoore::LastIndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, int offset) {
    int lastNeedleIndex = -1;
    int needleIndex;
    do {
        needleIndex = IndexOf(haystack, needle, offset);
        offset = needleIndex + 1;
        if (needleIndex >= 0) {
            lastNeedleIndex = needleIndex;
        }
    } while (needleIndex >= 0 && offset < static_cast<int>(haystack.size()));
    return lastNeedleIndex;
}

int BoyerMoore::IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, const std::vector<int>& byteTable, const std::vector<int>& offsetTable, bool ignoreCase, int offset) {
    if (!ignoreCase) return IndexOf(haystack, needle, byteTable, offsetTable, offset);

    if (needle.empty()) return 0;

    for (int i = offset + static_cast<int>(needle.size()) - 1, j; i < static_cast<int>(haystack.size()); ) {
        for (j = static_cast<int>(needle.size()) - 1; ToUpper(needle[j]) == ToUpper(haystack[i]); --i, --j) {
            if (j == 0) return i;
        }
        i += std::max(offsetTable[needle.size() - 1 - j], byteTable[ToUpper(haystack[i])]);
    }
    return -1;
}

int BoyerMoore::IndexOf(const std::vector<uint8_t>& haystack, const std::vector<uint8_t>& needle, const std::vector<int>& byteTable, const std::vector<int>& offsetTable, int offset) {
    if (needle.empty()) return 0;
    
    for (int i = offset + static_cast<int>(needle.size()) - 1, j; i < static_cast<int>(haystack.size()); ) {
        for (j = static_cast<int>(needle.size()) - 1; needle[j] == haystack[i]; --i, --j) {
            if (j == 0) return i;
        }
        i += std::max(offsetTable[needle.size() - 1 - j], byteTable[haystack[i]]);
    }
    return -1;
}

std::vector<uint8_t> BoyerMoore::ToUpper(const std::vector<uint8_t>& b) {
    std::vector<uint8_t> result(b.size());
    for (size_t i = 0; i < b.size(); ++i) {
        result[i] = ToUpper(b[i]);
    }
    return result;
}

uint8_t BoyerMoore::ToUpper(uint8_t b) {
    if (b < 0x61 || (b > 0x7a && b < 0x80)) {
        return b;
    }
    return static_cast<uint8_t>(b - 0x20);
}

std::vector<int> BoyerMoore::MakeByteTable(const std::vector<uint8_t>& needle, bool ignoreCase) {
    if (!ignoreCase) return MakeByteTable(needle);
    
    const int ALPHABET_SIZE = 256;
    std::vector<int> table(ALPHABET_SIZE, static_cast<int>(needle.size()));
    
    for (size_t i = 0; i < needle.size() - 1; ++i) {
        table[ToUpper(needle[i])] = static_cast<int>(needle.size() - 1 - i);
    }
    return table;
}

std::vector<int> BoyerMoore::MakeByteTable(const std::vector<uint8_t>& needle) {
    const int ALPHABET_SIZE = 256;
    std::vector<int> table(ALPHABET_SIZE, static_cast<int>(needle.size()));
    
    for (size_t i = 0; i < needle.size() - 1; ++i) {
        table[needle[i]] = static_cast<int>(needle.size() - 1 - i);
    }
    return table;
}

std::vector<int> BoyerMoore::MakeOffsetTable(const std::vector<uint8_t>& needle) {
    std::vector<int> table(needle.size());
    int lastPrefixPosition = static_cast<int>(needle.size());
    
    for (int i = static_cast<int>(needle.size()) - 1; i >= 0; --i) {
        if (isPrefix(needle, i + 1)) {
            lastPrefixPosition = i + 1;
        }
        table[needle.size() - 1 - i] = lastPrefixPosition - i + static_cast<int>(needle.size()) - 1;
    }
    for (size_t i = 0; i < needle.size() - 1; ++i) {
        int slen = suffixLength(needle, static_cast<int>(i));
        table[slen] = static_cast<int>(needle.size() - 1 - i) + slen;
    }
    return table;
}

bool BoyerMoore::isPrefix(const std::vector<uint8_t>& needle, int p) {
    for (size_t i = p, j = 0; i < needle.size(); ++i, ++j) {
        if (needle[i] != needle[j]) {
            return false;
        }
    }
    return true;
}

int BoyerMoore::suffixLength(const std::vector<uint8_t>& needle, int p) {
    int len = 0;
    for (int i = p, j = static_cast<int>(needle.size()) - 1;
         i >= 0 && needle[i] == needle[j]; --i, --j) {
        len += 1;
    }
    return len;
}

} // namespace Utils
} // namespace PacketParser
