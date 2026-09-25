#pragma once
#include <vector>
#include <string>

namespace SharedUtils {

class BloomFilter {
protected:
    std::vector<bool> bitArray;
    int nHashFunctions;
    int indexMask;
    long long wordCount;
    bool caseSensitive;

    virtual std::vector<int> GetIndexes(const std::string& word) const;

public:
    static const int DEFAULT_BITS_PER_ELEMENT = 15;

    BloomFilter(const std::vector<std::string>& wordList, bool caseSensitive = false, int bitsPerElement = DEFAULT_BITS_PER_ELEMENT);
    BloomFilter(long long wordListSizeEstimate, bool caseSensitive = false, int bitsPerElement = DEFAULT_BITS_PER_ELEMENT);
    virtual ~BloomFilter() = default;

    bool GetCaseSensitive() const { return caseSensitive; }
    long long GetWordCount() const { return wordCount; }

    double CalculateOverlap(const BloomFilter& other) const;
    void MergeWith(const BloomFilter& other);
    
    virtual bool HasWord(const std::string& word) const;
    bool TryAddWord(const std::string& word);
    virtual void AddWord(const std::string& word);
    virtual void Clear();
};

}