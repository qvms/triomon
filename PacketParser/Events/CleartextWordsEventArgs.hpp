#pragma once
#include <vector>
#include <string>

namespace PacketParser {
namespace Events {

class CleartextWordsEventArgs {
public:
    std::vector<std::string> Words;
    int WordCharCount;
    int TotalByteCount;
    CleartextWordsEventArgs(const std::vector<std::string>& words, int wordCharCount, int totalByteCount) 
        : Words(words), WordCharCount(wordCharCount), TotalByteCount(totalByteCount) {}
};

}
}
