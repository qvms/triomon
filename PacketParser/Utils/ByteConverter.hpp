#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace PacketParser {
namespace Utils {

class ByteConverter {
public:
    enum class Encoding {
        Normal,
        TDS_password
    };

    static uint16_t ToUInt16(uint8_t a, uint8_t b, bool reverseOrder);
    static uint16_t ToUInt16(const std::vector<uint8_t>& data, int offset, bool reverseOrder);
    static uint16_t ToUInt16(const uint8_t* data, int offset, bool reverseOrder);

    static uint32_t ToUInt32(const std::vector<uint8_t>& data, int offset, bool reverseOrder);
    static uint32_t ToUInt32(const uint8_t* data, int offset, bool reverseOrder);
    static uint32_t ToUInt32(uint16_t ushort1, uint16_t ushort2);

    static std::string ReadLine(const std::vector<uint8_t>& data, int& dataIndex);
    static std::string ReadLine(const uint8_t* data, int dataLength, int& dataIndex);

    static std::string ReadHexString(const std::vector<uint8_t>& data, int nBytesToRead, int offset = 0);
    static std::string ReadHexString(const uint8_t* data, int nBytesToRead, int offset = 0);

    static std::string ReadString(const std::vector<uint8_t>& data);
    static std::string ReadString(const std::vector<uint8_t>& data, int startIndex, int length);
    static std::string ReadString(const std::vector<uint8_t>& data, int startIndex, int length, bool unicodeData, bool reverseOrder);
    static std::string ReadString(const std::vector<uint8_t>& data, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder);
    static std::string ReadString(const std::vector<uint8_t>& data, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder, Encoding encoding);
    
    static std::string ReadString(const uint8_t* data, int dataLength, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder, Encoding encoding);

    static std::string ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex);
    static std::string ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex, bool unicodeData, bool reverseOrder);
    static std::string ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex, bool unicodeData, bool reverseOrder, int maxStringLength);
    static std::string ReadNullTerminatedString(const uint8_t* data, int dataLength, int& dataIndex, bool unicodeData, bool reverseOrder, int maxStringLength);

    static double StringToClosestDouble(const std::string& numberLikeLookingString);
    static std::string ToMd5HashString(const std::string& originalText);
    static uint16_t SwapNibbles(uint16_t data);
};

} // namespace Utils
} // namespace PacketParser
