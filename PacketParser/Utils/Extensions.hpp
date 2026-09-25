#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include "BoyerMoore.hpp"

namespace PacketParser {
namespace Utils {

class Extensions {
public:
    static bool StartsWith(const std::vector<uint8_t>& self, const std::vector<uint8_t>& signature, int offset = 0) {
        if (signature.size() > self.size() - offset) {
            return false;
        }
        for (size_t i = 0; i < signature.size(); ++i) {
            if (self[offset + i] != signature[i]) {
                return false;
            }
        }
        return true;
    }

    static int IndexOf(const std::vector<uint8_t>& self, const std::vector<uint8_t>& signature, int offset = 0) {
        return BoyerMoore::IndexOf(self, signature, offset);
    }

    // ReadTo requires stream and KnuthMorrisPratt - stubbed as a simple placeholder interface
    // to maintain exact class mapping
    static long long ReadTo(std::istream& stream, const std::vector<uint8_t>& signature, std::vector<uint8_t>& readBytes) {
        // Simple mock of KMP Stream reader
        return -1;
    }
};

} // namespace Utils
} // namespace PacketParser