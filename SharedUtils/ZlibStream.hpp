#pragma once
#include <vector>
#include <cstdint>
#include <memory>

// Mock of ArraySegmentStream for linking purposes in ZlibStream
namespace SharedUtils {

class ArraySegmentStream {
public:
    virtual ~ArraySegmentStream() = default;
    virtual void Write(const uint8_t* buffer, int offset, int count) = 0;
    virtual long long get_Length() const = 0;
    virtual bool get_CanWrite() const = 0;
    virtual void Flush() = 0;
};

class ZlibStream {
private:
    std::unique_ptr<ArraySegmentStream> gzipInputStream;
    // We would use something like z_stream from zlib in a full C++ app
    // std::unique_ptr<z_stream> gzipOutputStream;

public:
    static const std::vector<uint8_t> ZLIB_HEADER;
    static const std::vector<uint8_t> GZIP_HEADER;

    ZlibStream();
    ~ZlibStream();

    bool get_CanRead() const;
    bool get_CanSeek() const;
    bool get_CanWrite() const;
    long long get_Length() const;
    long long get_Position() const;

    void Flush();
    int Read(uint8_t* buffer, int offset, int count);
    void Write(const uint8_t* buffer, int offset, int count);
};

}