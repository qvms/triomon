#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include <stdexcept>

namespace PacketParser {
namespace Utils {

template <typename T>
class CuckooFilter {
public:
    using FingerprintFunction = std::function<uint8_t(const T&)>;

private:
    static const int MAX_KICKS = 100;
    uint32_t TABLE_SIZE;
    std::vector<int> table;

    struct HashedItem {
        uint32_t fullHashCode;
        uint32_t hash[2];
        uint8_t fingerprint;
        uint32_t tableSize;

        HashedItem(const T& item, uint32_t tableSize, const std::function<uint32_t(const T&)>& hasher) 
            : tableSize(tableSize) {
            fullHashCode = hasher(item);
            
            fingerprint = 0;
            for (int i = 24; i >= 0; i--) {
                fingerprint = static_cast<uint8_t>(fullHashCode >> i);
                if (fingerprint != 0) break;
            }
            if (fingerprint == 0) fingerprint = 0xff;

            hash[0] = fullHashCode % this->tableSize;
            hash[1] = hash[0] ^ (std::hash<uint8_t>{}(fingerprint) % this->tableSize);
        }
    };

    std::function<uint32_t(const T&)> hasher;

    int append(int bucket, uint8_t fingerprint) {
        return (bucket << 8) + fingerprint;
    }

    bool bucketContains(int bucket, uint8_t fingerprint) const {
        while (bucket != 0) {
            if (static_cast<uint8_t>(bucket) == fingerprint) {
                return true;
            }
            bucket >>= 8;
        }
        return false;
    }

    void kickMove(uint8_t fingerprint, uint32_t hashIndex, int maxRecursions) {
        if (!bucketContains(this->table[hashIndex], fingerprint)) {
            if (maxRecursions == 0) {
                throw std::runtime_error("Too many kicks in CuckooFilter!");
            }
            
            uint8_t kickedFingerprint = static_cast<uint8_t>(this->table[hashIndex] >> 24);
            this->table[hashIndex] = append(this->table[hashIndex], fingerprint);
            
            if (kickedFingerprint != 0) {
                kickMove(kickedFingerprint, hashIndex ^ (std::hash<uint8_t>{}(kickedFingerprint) % this->TABLE_SIZE), maxRecursions - 1);
            }
        }
    }

public:
    explicit CuckooFilter(int size, std::function<uint32_t(const T&)> hasher = std::hash<T>{}) 
        : hasher(hasher) {
        uint32_t roofSize = 1;
        while (roofSize < static_cast<uint32_t>(size)) {
            roofSize <<= 1;
        }
        this->TABLE_SIZE = roofSize;
        this->table.resize(this->TABLE_SIZE, 0);
    }

    bool Contains(const T& item) const {
        HashedItem hashedItem(item, this->TABLE_SIZE, hasher);
        for (int i = 0; i < 2; i++) {
            if (bucketContains(this->table[hashedItem.hash[i]], hashedItem.fingerprint)) {
                return true;
            }
        }
        return false;
    }

    void Insert(const T& item) {
        HashedItem hashedItem(item, this->TABLE_SIZE, hasher);
        int buckets[2];
        for (int i = 0; i < 2; i++) {
            buckets[i] = this->table[hashedItem.hash[i]];
        }

        for (int i = 0; i < 2; i++) {
            if ((buckets[i] >> 24) == 0) { // room in bucket
                this->table[hashedItem.hash[i]] = append(buckets[i], hashedItem.fingerprint);
                return;
            }
        }
        
        kickMove(hashedItem.fingerprint, hashedItem.hash[hashedItem.fingerprint % 1], MAX_KICKS);
    }
};

} // namespace Utils
} // namespace PacketParser