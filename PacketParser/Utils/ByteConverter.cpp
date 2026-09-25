#include "ByteConverter.hpp"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cctype>
#include <QCryptographicHash>
#include <QString>
#include <QByteArray>

namespace PacketParser {
namespace Utils {

uint16_t ByteConverter::ToUInt16(uint8_t a, uint8_t b, bool reverseOrder) {
    if (reverseOrder) {
        return static_cast<uint16_t>((b << 8) | a);
    } else {
        return static_cast<uint16_t>((a << 8) | b);
    }
}

uint16_t ByteConverter::ToUInt16(const std::vector<uint8_t>& data, int offset, bool reverseOrder) {
    return ToUInt16(data.data(), offset, reverseOrder);
}

uint16_t ByteConverter::ToUInt16(const uint8_t* data, int offset, bool reverseOrder) {
    return ToUInt16(data[offset], data[offset + 1], reverseOrder);
}

uint32_t ByteConverter::ToUInt32(const std::vector<uint8_t>& data, int offset, bool reverseOrder) {
    return ToUInt32(data.data(), offset, reverseOrder);
}

uint32_t ByteConverter::ToUInt32(const uint8_t* data, int offset, bool reverseOrder) {
    if (reverseOrder) {
        return (static_cast<uint32_t>(data[offset + 3]) << 24) |
               (static_cast<uint32_t>(data[offset + 2]) << 16) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               static_cast<uint32_t>(data[offset]);
    } else {
        return (static_cast<uint32_t>(data[offset]) << 24) |
               (static_cast<uint32_t>(data[offset + 1]) << 16) |
               (static_cast<uint32_t>(data[offset + 2]) << 8) |
               static_cast<uint32_t>(data[offset + 3]);
    }
}

uint32_t ByteConverter::ToUInt32(uint16_t ushort1, uint16_t ushort2) {
    uint32_t returnValue = ushort1;
    returnValue <<= 16;
    returnValue ^= ushort2;
    return returnValue;
}

std::string ByteConverter::ReadLine(const std::vector<uint8_t>& data, int& dataIndex) {
    return ReadLine(data.data(), static_cast<int>(data.size()), dataIndex);
}

std::string ByteConverter::ReadLine(const uint8_t* data, int dataLength, int& dataIndex) {
    int maxStringLength = 16384;
    std::string line;
    bool carriageReturnReceived = false;
    bool lineFeedReceived = false;
    int indexOffset = 0;
    while (!carriageReturnReceived || !lineFeedReceived) {
        if (dataIndex + indexOffset >= dataLength || indexOffset >= maxStringLength) {
            return "";
        } else {
            uint8_t b = data[dataIndex + indexOffset];
            if (b == 0x0d) {
                carriageReturnReceived = true;
            } else if (carriageReturnReceived && b == 0x0a) {
                lineFeedReceived = true;
            } else {
                line += static_cast<char>(b);
                carriageReturnReceived = false;
                lineFeedReceived = false;
            }
            indexOffset++;
        }
    }
    dataIndex += indexOffset;
    return line;
}

std::string ByteConverter::ReadHexString(const std::vector<uint8_t>& data, int nBytesToRead, int offset) {
    return ReadHexString(data.data(), nBytesToRead, offset);
}

std::string ByteConverter::ReadHexString(const uint8_t* data, int nBytesToRead, int offset) {
    std::ostringstream sb;
    sb << std::hex << std::setfill('0');
    for (int i = 0; i < nBytesToRead; i++) {
        sb << std::setw(2) << static_cast<int>(data[offset + i]);
    }
    return sb.str();
}

std::string ByteConverter::ReadString(const std::vector<uint8_t>& data) {
    int i = 0;
    return ReadString(data, i, static_cast<int>(data.size()), false, false, Encoding::Normal);
}

std::string ByteConverter::ReadString(const std::vector<uint8_t>& data, int startIndex, int length) {
    int i = startIndex;
    return ReadString(data, i, length, false, false, Encoding::Normal);
}

std::string ByteConverter::ReadString(const std::vector<uint8_t>& data, int startIndex, int length, bool unicodeData, bool reverseOrder) {
    int i = startIndex;
    return ReadString(data, i, length, unicodeData, reverseOrder, Encoding::Normal);
}

std::string ByteConverter::ReadString(const std::vector<uint8_t>& data, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder) {
    return ReadString(data, dataIndex, bytesToRead, unicodeData, reverseOrder, Encoding::Normal);
}

std::string ByteConverter::ReadString(const std::vector<uint8_t>& data, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder, Encoding encoding) {
    return ReadString(data.data(), static_cast<int>(data.size()), dataIndex, bytesToRead, unicodeData, reverseOrder, encoding);
}

std::string ByteConverter::ReadString(const uint8_t* data, int dataLength, int& dataIndex, int bytesToRead, bool unicodeData, bool reverseOrder, Encoding encoding) {
    int i = 0;
    std::string sb;
    while (i < bytesToRead && dataIndex + i < dataLength) {
        if (unicodeData) {
            if (dataIndex + i + 1 >= dataLength) break;
            uint16_t unicodeValue = ToUInt16(data, dataIndex + i, reverseOrder);
            if (encoding == Encoding::TDS_password) {
                unicodeValue ^= 0xa5a5;
                unicodeValue = SwapNibbles(unicodeValue);
            }
            sb += static_cast<char>(unicodeValue);
            i += 2;
        } else {
            sb += static_cast<char>(data[dataIndex + i]);
            i++;
        }
    }
    dataIndex += i;
    return sb;
}

std::string ByteConverter::ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex) {
    return ReadNullTerminatedString(data, dataIndex, false, false);
}

std::string ByteConverter::ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex, bool unicodeData, bool reverseOrder) {
    int maxStringLength = 1024;
    return ReadNullTerminatedString(data, dataIndex, unicodeData, reverseOrder, maxStringLength);
}

std::string ByteConverter::ReadNullTerminatedString(const std::vector<uint8_t>& data, int& dataIndex, bool unicodeData, bool reverseOrder, int maxStringLength) {
    return ReadNullTerminatedString(data.data(), static_cast<int>(data.size()), dataIndex, unicodeData, reverseOrder, maxStringLength);
}

std::string ByteConverter::ReadNullTerminatedString(const uint8_t* data, int dataLength, int& dataIndex, bool unicodeData, bool reverseOrder, int maxStringLength) {
    std::string returnString;

    if (!unicodeData) {
        for (int offset = 0; dataIndex + offset < dataLength && offset < maxStringLength; offset++) {
            uint8_t b = data[dataIndex + offset];
            if (b == 0x00) {
                dataIndex += (offset + 1);
                return returnString;
            } else {
                returnString += static_cast<char>(b);
            }
        }
    } else { // unicode
        for (int offset = 0; dataIndex + offset < dataLength && offset < maxStringLength * 2; offset += 2) {
            uint16_t b;
            if (dataIndex + offset + 1 < dataLength) {
                b = ToUInt16(data, dataIndex + offset, reverseOrder);
            } else { // only one byte to read
                b = static_cast<uint16_t>(data[dataIndex + offset]);
            }
            if (b == 0x0000) {
                dataIndex += (offset + 2);
                return returnString;
            } else {
                returnString += static_cast<char>(b);
            }
        }
    }
    return returnString;
}

double ByteConverter::StringToClosestDouble(const std::string& numberLikeLookingString) {
    double returnValue = 0.0;
    int decimalNumber = 0;
    for (size_t i = 0; i < numberLikeLookingString.length(); i++) {
        char c = numberLikeLookingString[i];
        if (std::isdigit(c)) {
            if (decimalNumber == 0) {
                returnValue = returnValue * 10 + (c - '0');
            } else {
                returnValue += (c - '0') / std::pow(10.0, decimalNumber);
                decimalNumber++;
            }
        } else if (decimalNumber == 0 && (c == '.' || c == ',')) {
            decimalNumber = 1;
        }
    }
    return returnValue;
}

std::string ByteConverter::ToMd5HashString(const std::string& originalText) {
    QByteArray hashArray = QCryptographicHash::hash(
        QByteArray::fromStdString(originalText), QCryptographicHash::Md5);
    return QString(hashArray.toHex()).toStdString();
}

uint16_t ByteConverter::SwapNibbles(uint16_t data) {
    return static_cast<uint16_t>(((data >> 4) & 0x0f0f) | ((data << 4) & 0xf0f0));
}

} // namespace Utils
} // namespace PacketParser
