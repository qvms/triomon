#pragma once

#include <string>
#include <map>
#include <QRegularExpression>

namespace PacketHandlerFramework {
namespace Fingerprints {

class DictionaryFactory {
public:
    static std::map<std::string, std::string> CreateDictionaryFromTrisulJa3Json(const std::string& jsonDictionaryFile);
    static std::map<std::string, std::string> CreateDictionaryFromLineRegex(const QRegularExpression& regex, const std::string& keyName, const std::string& valueName, const std::string& filePath);
    static std::map<std::string, std::string> CreateDictionaryFromCsv(const std::string& csvFile, int keyColumn, int valueColumn, bool skipFirstLine = false);
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
