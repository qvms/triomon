#pragma once
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <cstdint>

namespace SharedUtils {

class MathUtils {
public:
    static double GetMedian(std::vector<double> values) {
        if (values.empty()) return 0.0;
        size_t size = values.size();
        size_t middleIndex = (size - 1) / 2;
        std::sort(values.begin(), values.end());
        if (size % 2 == 0) {
            return (values[middleIndex] + values[middleIndex + 1]) / 2.0;
        } else {
            return values[middleIndex];
        }
    }

    template <typename T>
    static T GetKeyWithMaxCount(const std::map<T, int>& keyCounterDict, double& certainty) {
        if (keyCounterDict.empty()) {
            certainty = 0.0;
            return T();
        }

        int totalCounts = 0;
        int bestCount = 0;
        T mostCommonKey = T();

        for (const auto& kvp : keyCounterDict) {
            totalCounts += kvp.second;
            if (kvp.second > bestCount) {
                bestCount = kvp.second;
                mostCommonKey = kvp.first;
            }
        }
        if (totalCounts == 0) {
            certainty = 0.0;
            return T();
        } else {
            certainty = (1.0 * bestCount) / totalCounts;
            return mostCommonKey;
        }
    }

    static double GetEntropy(const uint8_t* data, int offset, int length) {
        if (length < 1) return 0.0;
        double entropy = 0.0;
        int byteCount[256] = {0};
        for (int i = 0; i < length; i++) {
            byteCount[data[offset + i]]++;
        }
        for (int i = 0; i < 256; i++) {
            double byteFreq = (1.0 * byteCount[i]) / length;
            if (byteFreq > 0.0) {
                entropy -= byteFreq * std::log2(byteFreq);
            }
        }
        return entropy;
    }

    static uint8_t CountBitsInMask(uint32_t mask) {
        if (mask == 0) return 0;
        uint8_t bits = 1;
        while ((1ULL << bits) < mask) {
            bits++;
        }
        return bits;
    }
};

}