#include "KnuthMorrisPratt.hpp"
#include <sstream>

namespace PacketParser {
namespace Utils {

long long KnuthMorrisPratt::ReadTo(const std::vector<uint8_t>& pattern, const std::vector<uint8_t>& data, int offset, std::vector<uint8_t>& readBytes) {
    return ReadTo(pattern, data, offset, readBytes, KmpFailureFunction(pattern));
}

long long KnuthMorrisPratt::ReadTo(const std::vector<uint8_t>& pattern, const std::vector<uint8_t>& data, int offset, std::vector<uint8_t>& readBytes, const std::vector<int>& kmpFailureFunction) {
    std::string strData(reinterpret_cast<const char*>(data.data()), data.size());
    std::istringstream stream(strData);
    stream.seekg(offset);
    return ReadTo(pattern, stream, readBytes, kmpFailureFunction);
}

long long KnuthMorrisPratt::ReadTo(const std::vector<uint8_t>& pattern, std::istream& stream, std::vector<uint8_t>& readBytes) {
    return ReadTo(pattern, stream, readBytes, KmpFailureFunction(pattern));
}

long long KnuthMorrisPratt::ReadTo(const std::vector<uint8_t>& pattern, std::istream& stream, std::vector<uint8_t>& readBytes, const std::vector<int>& kmpFailureFunction) {
    readBytes.clear();
    
    // get stream length
    std::streampos originalPos = stream.tellg();
    stream.seekg(0, std::ios::end);
    std::streampos streamLength = stream.tellg();
    stream.seekg(originalPos);
    
    int j = 0;
    int tmp = stream.get();
    if (tmp == EOF) {
        return -1; // end of stream
    }
    
    uint8_t t = static_cast<uint8_t>(tmp);
    readBytes.push_back(t);
    
    while (stream.tellg() <= streamLength) {
        if (pattern[j] == t) {
            if (j == static_cast<int>(pattern.size()) - 1) {
                return static_cast<long long>(stream.tellg()) - static_cast<long long>(pattern.size());
            }
            tmp = stream.get();
            if (tmp == EOF) {
                return -1;
            }
            t = static_cast<uint8_t>(tmp);
            readBytes.push_back(t);
            j++;
        } else if (j > 0) {
            j = kmpFailureFunction[j - 1];
        } else {
            tmp = stream.get();
            if (tmp == EOF) {
                return -1;
            }
            t = static_cast<uint8_t>(tmp);
            readBytes.push_back(t);
        }
    }
    
    return -1;
}

std::vector<int> KnuthMorrisPratt::KmpFailureFunction(const std::vector<uint8_t>& pattern) {
    int i = 1;
    int j = 0;
    std::vector<int> f(pattern.size(), 0);
    
    while (i < static_cast<int>(pattern.size())) {
        if (pattern[j] == pattern[i]) {
            f[i] = j + 1;
            i++;
            j++;
        } else if (j > 0) {
            j = f[j - 1];
        } else {
            f[i] = 0;
            i++;
        }
    }
    
    return f;
}

} // namespace Utils
} // namespace PacketParser
