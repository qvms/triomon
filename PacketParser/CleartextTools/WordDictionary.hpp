#pragma once

#include <string>
#include <vector>
#include <bitset>
#include <memory>
#include <cstdint>
#include "SharedUtils/BloomFilter.hpp"

namespace PacketParser {
namespace CleartextDictionary {

class WordDictionary {
private:
    int longestWord;
    int minWordLength;
    std::unique_ptr<SharedUtils::BloomFilter> bloomFilter;
    std::bitset<256> byteLetters; // holds true if there is a letter at the position

    void AddWord(const std::string& word, std::vector<std::string>& wordList);

public:
    WordDictionary();
    ~WordDictionary() = default;

    int GetLongestWord() const { return longestWord; }

    void LoadDictionaryFile(const std::string& dictionaryFile);

    bool HasWord(const std::string& word) const;
    bool IsLetter(uint8_t b) const;
};

} // namespace CleartextDictionary
} // namespace PacketParser
