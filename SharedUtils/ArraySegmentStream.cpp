#include "ArraySegmentStream.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace SharedUtils {

ArraySegmentStream::ArraySegmentStream() : length(0), position(0) {
}

int ArraySegmentStream::Read(uint8_t* buffer, int offset, int count) {
    if (position >= length) return 0;

    int bytesRead = 0;
    long long currentPos = 0;

    for (const auto& segment : segments) {
        long long segmentEnd = currentPos + segment.size();
        
        if (position >= currentPos && position < segmentEnd) {
            int offsetInSegment = static_cast<int>(position - currentPos);
            int availableInSegment = static_cast<int>(segment.size() - offsetInSegment);
            int toRead = std::min(count - bytesRead, availableInSegment);

            std::memcpy(buffer + offset + bytesRead, segment.data() + offsetInSegment, toRead);
            
            bytesRead += toRead;
            position += toRead;

            if (bytesRead >= count) break;
        }
        currentPos = segmentEnd;
    }
    
    return bytesRead;
}

void ArraySegmentStream::Write(const uint8_t* buffer, int offset, int count) {
    if (count <= 0) return;
    
    std::vector<uint8_t> newSegment(buffer + offset, buffer + offset + count);
    segments.push_back(std::move(newSegment));
    length += count;
    position += count;
}

}
