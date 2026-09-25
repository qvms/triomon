#pragma once
#include <vector>
#include <cstdint>

// Forward declaring standard stream like concepts
namespace SharedUtils {

class ArraySegmentStream {
private:
    std::vector<std::vector<uint8_t>> segments;
    long long length;
    long long position;

public:
    ArraySegmentStream();
    ~ArraySegmentStream() = default;

    bool get_CanRead() const { return true; }
    bool get_CanSeek() const { return false; }
    bool get_CanWrite() const { return true; }
    
    long long get_Length() const { return length; }
    long long get_Position() const { return position; }
    void set_Position(long long pos) { position = pos; }

    void Flush() {}
    
    int Read(uint8_t* buffer, int offset, int count);
    void Write(const uint8_t* buffer, int offset, int count);
};

}