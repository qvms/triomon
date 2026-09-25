#include "BloomFilterExportable.hpp"
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <QByteArray>
#include <QCryptographicHash>

namespace SharedUtils {

void BloomFilterExportable::InitHashCreator() {
    hashOffsetMultiplicator = 4;
    if (nHashFunctions > 64) {
        throw std::runtime_error("Too many hash functions.");
    }
    if (nHashFunctions > 16) {
        hashOffsetMultiplicator = 5;
    }
}

BloomFilterExportable::BloomFilterExportable(const std::vector<std::string>& wordList, bool caseSensitive)
    : BloomFilter(wordList, caseSensitive, DEFAULT_BITS_PER_ELEMENT) {
    InitHashCreator();
}

BloomFilterExportable::BloomFilterExportable(long long wordListSizeEstimate, bool caseSensitive)
    : BloomFilter(wordListSizeEstimate, caseSensitive, DEFAULT_BITS_PER_ELEMENT) {
    InitHashCreator();
}

std::vector<int> BloomFilterExportable::GetIndexes(const std::string& word) const {
    std::string w = word;
    if (!caseSensitive) {
        std::transform(w.begin(), w.end(), w.begin(), ::tolower);
    }
    std::vector<int> indexes(nHashFunctions);
    
    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;
    if (nHashFunctions > 4 && nHashFunctions <= 8) algo = QCryptographicHash::Sha256;
    else if (nHashFunctions > 8 && nHashFunctions <= 12) algo = QCryptographicHash::Sha384;
    else if (nHashFunctions > 12) algo = QCryptographicHash::Sha512;

    QByteArray hash = QCryptographicHash::hash(QByteArray::fromStdString(w), algo);
    const uint8_t* hashData = reinterpret_cast<const uint8_t*>(hash.constData());
    int hashLen = hash.length();

    for (int i = 0; i < nHashFunctions; i++) {
        int hashIndex = i * hashOffsetMultiplicator;
        indexes[i] = ((hashData[hashIndex % hashLen] << 24) |
                      (hashData[(hashIndex + 1) % hashLen] << 16) |
                      (hashData[(hashIndex + 2) % hashLen] << 8) |
                      hashData[(hashIndex + 3) % hashLen]) & indexMask;
    }
    return indexes;
}

std::string BloomFilterExportable::ExportAsBase64() const {
    int bytesCount = (bitArray.size() + 7) / 8;
    QByteArray bytes(bytesCount, 0);
    for (size_t i = 0; i < bitArray.size(); ++i) {
        if (bitArray[i]) {
            bytes[i / 8] = bytes[i / 8] | (1 << (i % 8));
        }
    }
    return bytes.toBase64().toStdString();
}

BloomFilterExportable BloomFilterExportable::CreateFromBase64(const std::string& base64, long long wordListSizeEstimate, bool caseSensitive) {
    BloomFilterExportable filter(wordListSizeEstimate, caseSensitive);
    QByteArray bytes = QByteArray::fromBase64(QByteArray::fromStdString(base64));
    for (size_t i = 0; i < filter.bitArray.size(); ++i) {
        if (i / 8 < static_cast<size_t>(bytes.size())) {
            if (bytes[i / 8] & (1 << (i % 8))) {
                filter.bitArray[i] = true;
            }
        }
    }
    return filter;
}

}
