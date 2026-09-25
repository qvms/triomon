#include "ByteArrayStream.hpp"
#include <cstring>
#include <algorithm>

namespace PacketParser {
namespace Mime {

ByteArrayStream::ByteArrayStream(const std::vector<uint8_t>& data, long long startIndex) 
    : data(data), index(startIndex) {
}

int ByteArrayStream::Read(uint8_t* buffer, int offset, int count) {
    if (index >= data.size()) {
        return 0; // end of stream reached
    }
    if (count <= 0) {
        return 0;
    }

    if (data.size() < static_cast<size_t>(index + count)) {
        count = static_cast<int>(data.size() - index);
    }
    
    // We assume buffer is large enough as we don't have C#'s array boundary checks here directly
    // Caller is responsible for offset/count bounds checking on their buffer.
    
    std::memcpy(buffer + offset, data.data() + index, count);
    index += count;
    return count;
}

long long ByteArrayStream::Seek(long long offset, SeekOrigin origin) {
    if (origin == SeekOrigin::Begin) {
        this->index = offset;
    } else if (origin == SeekOrigin::End) {
        this->index = data.size() + offset;
    } else if (origin == SeekOrigin::Current) {
        this->index += offset;
    }
    return this->index;
}

} // namespace Mime
} // namespace PacketParser
