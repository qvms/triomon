#pragma once

namespace PacketParser {
namespace FileTransfer {

class ContentRange {
public:
    long long Start = 0;
    long long End = 0; // last index inside the content range
    long long Total = 0;
};

} // namespace FileTransfer
} // namespace PacketParser
