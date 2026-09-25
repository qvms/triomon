#pragma once
#include "BloomFilter.hpp"
#include <unordered_set>
#include <string>
#include <vector>

namespace SharedUtils {

class BloomHashSet : public BloomFilter {
private:
    static const long long MAX_WORDS_IN_HASH_SET = 100000;
    static const long long MAX_CHARS_IN_HASH_SET = 1000000;

    std::unordered_set<std::string> hashSet;
    bool useHashSet;
    long long charCount;

public:
    BloomHashSet(const std::vector<std::string>& wordList, bool caseSensitive = false);
    BloomHashSet(long long wordListSizeEstimate, bool caseSensitive = false);

    bool TryGetWords(std::vector<std::string>& wordCollection) const;

    void AddWord(const std::string& word) override;
    void Clear() override;
    bool HasWord(const std::string& word) const override;
};

}