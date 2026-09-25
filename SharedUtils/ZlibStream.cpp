#include "ZlibStream.hpp"
#include <stdexcept>

namespace SharedUtils {

const std::vector<uint8_t> ZlibStream::ZLIB_HEADER = { 0x78, 0x01 };
const std::vector<uint8_t> ZlibStream::GZIP_HEADER = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

ZlibStream::ZlibStream() {
    // In C++ we might use a QBuffer or stringstream instead of ArraySegmentStream
    // For this mock implementation, we just throw if the methods are called
}

ZlibStream::~ZlibStream() {
}

bool ZlibStream::get_CanRead() const {
    return true;
}

bool ZlibStream::get_CanSeek() const {
    return false;
}

bool ZlibStream::get_CanWrite() const {
    return gzipInputStream ? gzipInputStream->get_CanWrite() : false;
}

long long ZlibStream::get_Length() const {
    throw std::runtime_error("Not implemented");
}

long long ZlibStream::get_Position() const {
    throw std::runtime_error("Not implemented");
}

void ZlibStream::Flush() {
    if (gzipInputStream) gzipInputStream->Flush();
}

int ZlibStream::Read(uint8_t* buffer, int offset, int count) {
    // Return mock 0 bytes read
    return 0;
}

void ZlibStream::Write(const uint8_t* buffer, int offset, int count) {
    if (!gzipInputStream) return;
    
    if (gzipInputStream->get_Length() == static_cast<long long>(GZIP_HEADER.size())) {
        if (buffer[0] == ZLIB_HEADER[0]) {
            offset += ZLIB_HEADER.size();
            count -= ZLIB_HEADER.size();
        } else {
            throw std::invalid_argument("Input buffer must be ZLIB compressed");
        }
    }
    if (count > 0) {
        gzipInputStream->Write(buffer, offset, count);
    }
}

}
