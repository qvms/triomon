#include "BloomHashSet.hpp"
#include <algorithm>
#include <cctype>

namespace SharedUtils {

BloomHashSet::BloomHashSet(const std::vector<std::string>& wordList, bool caseSensitive)
    : BloomFilter(wordList, caseSensitive), useHashSet(true), charCount(0) {
}

BloomHashSet::BloomHashSet(long long wordListSizeEstimate, bool caseSensitive)
    : BloomFilter(wordListSizeEstimate, caseSensitive), useHashSet(true), charCount(0) {
}

bool BloomHashSet::TryGetWords(std::vector<std::string>& wordCollection) const {
    if (useHashSet) {
        wordCollection.assign(hashSet.begin(), hashSet.end());
        return true;
    }
    return false;
}

void BloomHashSet::AddWord(const std::string& word) {
    if (useHashSet) {
        if (wordCount > MAX_WORDS_IN_HASH_SET || charCount > MAX_CHARS_IN_HASH_SET) {
            useHashSet = false;
            hashSet.clear();
        } else {
            if (caseSensitive) {
                hashSet.insert(word);
            } else {
                std::string lowerWord = word;
                std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                hashSet.insert(lowerWord);
            }
            charCount += word.length();
        }
    }
    BloomFilter::AddWord(word);
}

void BloomHashSet::Clear() {
    BloomFilter::Clear();
    charCount = 0;
    useHashSet = true;
    hashSet.clear();
}

bool BloomHashSet::HasWord(const std::string& word) const {
    if (BloomFilter::HasWord(word)) {
        if (!useHashSet) {
            return true;
        } else if (caseSensitive) {
            return hashSet.find(word) != hashSet.end();
        } else {
            std::string lowerWord = word;
            std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            return hashSet.find(lowerWord) != hashSet.end();
        }
    }
    return false;
}

}
