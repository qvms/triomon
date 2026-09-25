#include "WordDictionary.hpp"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace PacketParser {
namespace CleartextDictionary {

WordDictionary::WordDictionary() :
    longestWord(0),
    minWordLength(3),
    byteLetters()
{
}

void WordDictionary::LoadDictionaryFile(const std::string& dictionaryFile) {
    std::vector<std::string> wordList;
    std::ifstream fileStream(dictionaryFile);
    
    if (!fileStream.is_open()) {
        return;
    }

    std::string line;
    const std::string separators = " ,.!?<>(){}[]\"\'";

    while (std::getline(fileStream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        size_t start = 0;
        size_t end = line.find_first_of(separators);
        
        while (end != std::string::npos) {
            AddWord(line.substr(start, end - start), wordList);
            start = end + 1;
            end = line.find_first_of(separators, start);
        }
        if (start <= line.length()) {
            AddWord(line.substr(start), wordList);
        }
    }
    
    bloomFilter = std::make_unique<SharedUtils::BloomFilter>(wordList);
}

void WordDictionary::AddWord(const std::string& word, std::vector<std::string>& wordList) {
    if (word.length() >= static_cast<size_t>(minWordLength)) {
        std::string lowerWord = word;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
            [](unsigned char c){ return std::tolower(c); });
        wordList.push_back(lowerWord);

        for (char c : word) {
            byteLetters.set(static_cast<uint8_t>(c));
        }

        std::string upperWord = word;
        std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(),
            [](unsigned char c){ return std::toupper(c); });

        for (char c : upperWord) {
            byteLetters.set(static_cast<uint8_t>(c));
        }

        if (word.length() > static_cast<size_t>(longestWord)) {
            longestWord = static_cast<int>(word.length());
        }
    }
}

bool WordDictionary::HasWord(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
        [](unsigned char c){ return std::tolower(c); });
        
    if (lowerWord.length() > static_cast<size_t>(longestWord) || 
        lowerWord.length() < static_cast<size_t>(minWordLength)) {
        return false;
    }
    
    if (bloomFilter) {
        return bloomFilter->HasWord(lowerWord);
    }
    return false;
}

bool WordDictionary::IsLetter(uint8_t b) const {
    return byteLetters.test(b);
}

} // namespace CleartextDictionary
} // namespace PacketParser
