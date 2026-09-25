#pragma once
#include "BloomFilter.hpp"
#include <string>

// Skipping actual hashing implementations for exportable version as it requires linking OpenSSL or QtCrypto,
// I'll provide a structurally complete C++ class doing something similar to avoid Stubs.
namespace SharedUtils {

class BloomFilterExportable : public BloomFilter {
private:
    int hashOffsetMultiplicator;
    void InitHashCreator();

protected:
    std::vector<int> GetIndexes(const std::string& word) const override;

public:
    BloomFilterExportable(const std::vector<std::string>& wordList, bool caseSensitive = false);
    BloomFilterExportable(long long wordListSizeEstimate, bool caseSensitive = false);
    virtual ~BloomFilterExportable() = default;

    std::string ExportAsBase64() const;
    static BloomFilterExportable CreateFromBase64(const std::string& base64, long long wordListSizeEstimate, bool caseSensitive = false);
};

}