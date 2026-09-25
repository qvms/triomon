#include "BloomFilter.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <functional>

namespace SharedUtils {

std::vector<int> BloomFilter::GetIndexes(const std::string& word) const {
    std::string w = word;
    if (!caseSensitive) {
        std::transform(w.begin(), w.end(), w.begin(),
                       [](unsigned char c){ return std::tolower(c); });
    }

    std::vector<int> indexes(nHashFunctions);
    std::hash<std::string> hasher;

    for (int i = 0; i < nHashFunctions; i++) {
        std::string s = w + std::to_string(i);
        int hash = static_cast<int>(hasher(s));
        indexes[i] = (hash * i * 7) & indexMask;
    }
    return indexes;
}

BloomFilter::BloomFilter(long long wordListSizeEstimate, bool caseSensitive, int bitsPerElement)
    : caseSensitive(caseSensitive), wordCount(0) {
    int indexValueBits = 0;
    while ((1ULL << indexValueBits) < static_cast<unsigned long long>(bitsPerElement * wordListSizeEstimate)) {
        indexValueBits++;
    }
    indexValueBits++;
    int indexSize = 1 << (indexValueBits - 1);
    indexMask = indexSize - 1;

    bitArray.resize(indexSize, false);
    if (wordListSizeEstimate == 0) {
        nHashFunctions = 1;
    } else {
        nHashFunctions = static_cast<int>(0.7 * indexSize / wordListSizeEstimate);
    }
}

BloomFilter::BloomFilter(const std::vector<std::string>& wordList, bool caseSensitive, int bitsPerElement)
    : BloomFilter(wordList.size(), caseSensitive, bitsPerElement) {
    for (const auto& s : wordList) {
        AddWord(s);
    }
}

double BloomFilter::CalculateOverlap(const BloomFilter& other) const {
    if (indexMask != other.indexMask || nHashFunctions != other.nHashFunctions) {
        throw std::runtime_error("Incompatible bloom filters");
    }
    if (wordCount < other.wordCount) {
        return other.CalculateOverlap(*this);
    } else if (other.wordCount == 0) {
        return 0.0;
    } else {
        long long setBitCount = 0;
        long long collisions = 0;
        for (size_t i = 0; i < bitArray.size(); i++) {
            if (bitArray[i]) {
                setBitCount++;
                if (other.bitArray[i]) {
                    collisions++;
                }
            }
        }
        if (setBitCount >= static_cast<long long>(bitArray.size())) {
            throw std::runtime_error("Bloom filter is full");
        }
        double fillRate = (1.0 * setBitCount) / bitArray.size();
        double expectedRandomCollisions = other.wordCount * fillRate;
        if (collisions < expectedRandomCollisions) return 0.0;
        return (collisions - expectedRandomCollisions) / (other.wordCount - expectedRandomCollisions);
    }
}

void BloomFilter::MergeWith(const BloomFilter& other) {
    for (size_t i = 0; i < bitArray.size(); ++i) {
        bitArray[i] = bitArray[i] | other.bitArray[i];
    }
    wordCount += other.wordCount;
}

bool BloomFilter::HasWord(const std::string& word) const {
    std::vector<int> indexes = GetIndexes(word);
    for (int index : indexes) {
        if (!bitArray[index]) return false;
    }
    return true;
}

bool BloomFilter::TryAddWord(const std::string& word) {
    if (wordCount < static_cast<long long>(bitArray.size()) / 2) {
        AddWord(word);
        return true;
    }
    return false;
}

void BloomFilter::AddWord(const std::string& word) {
    std::vector<int> indexes = GetIndexes(word);
    for (int index : indexes) {
        bitArray[index] = true;
    }
    wordCount++;
}

void BloomFilter::Clear() {
    std::fill(bitArray.begin(), bitArray.end(), false);
    wordCount = 0;
}

}
