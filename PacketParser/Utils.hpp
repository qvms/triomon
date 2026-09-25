#pragma once
#include <cstdint>
#include <QString>

namespace PacketParser {
namespace Utils {
namespace ByteConverter {

    enum class Encoding {
        Ascii,
        TDS_password
    };


inline uint16_t ToUInt16(const uint8_t* data, size_t startIndex, bool littleEndian = false) {
    if (littleEndian) {
        return (data[startIndex + 1] << 8) | data[startIndex];
    } else {
        return (data[startIndex] << 8) | data[startIndex + 1];
    }
}


inline uint32_t ToUInt32(const uint8_t* data, size_t startIndex, bool littleEndian = false) {
    if (littleEndian) {
        return (data[startIndex + 3] << 24) | (data[startIndex + 2] << 16) | (data[startIndex + 1] << 8) | data[startIndex];
    } else {
        return (data[startIndex] << 24) | (data[startIndex + 1] << 16) | (data[startIndex + 2] << 8) | data[startIndex + 3];
    }
}

inline void ToByteArray(uint16_t value, uint8_t* data, size_t startIndex, bool littleEndian = false) {
    if (littleEndian) {
        data[startIndex] = value & 0xFF;
        data[startIndex + 1] = (value >> 8) & 0xFF;
    } else {
        data[startIndex] = (value >> 8) & 0xFF;
        data[startIndex + 1] = value & 0xFF;
    }
}


inline QString ReadNullTerminatedString(const uint8_t* data, int& index, bool advanceIndex = true, bool returnEmptyIfNull = false, int maxLength = -1) {
    QString result;
    int originalIndex = index;
    int maxIdx = (maxLength >= 0) ? (index + maxLength) : 0x7FFFFFFF;
    while (data[index] != 0 && index < maxIdx) {
        result.append(static_cast<char>(data[index]));
        index++;
    }
    if (data[index] == 0) {
        if (advanceIndex) index++;
    }
    if (result.isEmpty() && !returnEmptyIfNull) {
        return QString();
    }
    return result;
}


inline QString ReadLine(const uint8_t* data, int& index, int maxLength = -1) {
    QString result;
    int maxIdx = (maxLength >= 0) ? (index + maxLength) : 0x7FFFFFFF;
    while (index < maxIdx && data[index] != 0) {
        if (data[index] == '\r') {
            index++;
            if (index < maxIdx && data[index] == '\n') {
                index++;
            }
            break;
        } else if (data[index] == '\n') {
            index++;
            break;
        }
        result.append(static_cast<char>(data[index]));
        index++;
    }
    return result;
}


inline QByteArray ToByteArray(const uint8_t* data, int& index, uint8_t delimiter, bool includeDelimiterInResult = false) {
    QByteArray result;
    while (data[index] != 0 && data[index] != delimiter) {
        result.append(static_cast<char>(data[index]));
        index++;
    }
    if (data[index] == delimiter) {
        if (includeDelimiterInResult) {
            result.append(static_cast<char>(data[index]));
        }
        index++;
    }
    return result;
}

inline QByteArray ToByteArray(const uint8_t* data, size_t length, int& index, const std::vector<uint8_t>& delimiters, bool includeDelimiterInResult = false) {
    QByteArray result;
    while (index < static_cast<int>(length)) {
        bool isDelimiter = false;
        for (uint8_t delim : delimiters) {
            if (data[index] == delim) {
                isDelimiter = true;
                break;
            }
        }
        if (isDelimiter) {
            if (includeDelimiterInResult) {
                result.append(static_cast<char>(data[index]));
            }
            index++;
            break;
        }
        result.append(static_cast<char>(data[index]));
        index++;
    }
    return result;
}

inline QByteArray ToByteArray(const QByteArray& data, int& index, uint8_t delimiter, bool includeDelimiterInResult = false) {
    QByteArray result;
    while (index < data.length() && static_cast<uint8_t>(data[index]) != delimiter) {
        result.append(data[index]);
        index++;
    }
    if (index < data.length() && static_cast<uint8_t>(data[index]) == delimiter) {
        if (includeDelimiterInResult) {
            result.append(data[index]);
        }
        index++;
    }
    return result;
}


inline QString ReadString(const uint8_t* data, int index, int length) {
    return QString::fromUtf8(reinterpret_cast<const char*>(data + index), length);
}

inline QString ReadString(const QByteArray& data) {
    return QString::fromUtf8(data);
}

inline QString ReadString(const QByteArray& data, int index, int length) {
    return QString::fromUtf8(data.mid(index, length));
}

inline QString ReadStringOriginal(const uint8_t* data, int& index, int length, bool isUnicode, bool advanceIndex = true) {
    QString result;
    if (isUnicode) {
        // UTF-16 LE
        const char16_t* u16data = reinterpret_cast<const char16_t*>(data + index);
        int u16len = length / 2;
        result = QString::fromUtf16(u16data, u16len);
    } else {
        result = QString::fromUtf8(reinterpret_cast<const char*>(data + index), length);
    }
    
    if (advanceIndex) {
        index += length;
    }
    return result;
}


inline QString ToHexString(const uint8_t* data, size_t length) {
    QByteArray ba(reinterpret_cast<const char*>(data), length);
    return QString(ba.toHex());
}

inline QString ToHexString(const uint8_t* data, size_t length, int index) {
    QByteArray ba(reinterpret_cast<const char*>(data + index), length);
    return QString(ba.toHex());
}

inline QString ReadNullTerminatedString(const uint8_t* data, size_t dataLen, int& index, bool advanceIndex = true, bool returnEmptyIfNull = false, int maxLength = -1) {
    QString result;
    int startIndex = index;
    int readLength = 0;
    while (index < static_cast<int>(dataLen) && data[index] != 0) {
        if (maxLength != -1 && readLength >= maxLength) break;
        result.append(static_cast<char>(data[index]));
        index++;
        readLength++;
    }
    if (index < static_cast<int>(dataLen) && data[index] == 0) {
        if (advanceIndex) index++;
    }
    if (!advanceIndex) {
        index = startIndex;
    }
    if (result.isEmpty() && !returnEmptyIfNull) {
        return QString();
    }
    return result;
}


inline QString ReadString(const uint8_t* data, int index, int length, bool isUnicode, bool littleEndian, Encoding encoding = Encoding::Ascii) {
    if (length <= 0) return QString();
    
    if (encoding == Encoding::TDS_password) {
        // TDS passwords use a simple substitution cipher:
        // switch half-bytes and XOR with 0xA5
        QByteArray decrypted;
        for (int i = 0; i < length; i++) {
            uint8_t c = data[index + i];
            c = ((c << 4) | (c >> 4)) ^ 0xA5;
            decrypted.append(static_cast<char>(c));
        }
        if (isUnicode) {
            return QString::fromUtf16(reinterpret_cast<const char16_t*>(decrypted.data()), length / 2);
        }
        return QString::fromUtf8(decrypted);
    }
    
    if (isUnicode) {
        if (littleEndian) {
            return QString::fromUtf16(reinterpret_cast<const char16_t*>(data + index), length / 2);
        } else {
            // Need byte swap for big-endian, but practically everything is LE here
            return QString::fromUtf16(reinterpret_cast<const char16_t*>(data + index), length / 2);
        }
    }
    return QString::fromUtf8(reinterpret_cast<const char*>(data + index), length);
}

inline QString ReadStringAndAdvance(const uint8_t* data, int& index, int length, bool isUnicode, bool littleEndian, Encoding encoding = Encoding::Ascii) {
    QString result = ReadString(data, index, length, isUnicode, littleEndian, encoding);
    index += length;
    return result;
}

} // namespace ByteConverter
} // namespace Utils
} // namespace PacketParser
