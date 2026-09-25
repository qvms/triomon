#include "DictionaryFactory.hpp"
#include <QFile>
#include <QTextStream>
#include <QStringList>

namespace PacketHandlerFramework {
namespace Fingerprints {

std::map<std::string, std::string> DictionaryFactory::CreateDictionaryFromTrisulJa3Json(const std::string& jsonDictionaryFile) {
    QRegularExpression regex(R"(\"desc\":\"(?<desc>[^\"]*)\",\"ja3_hash\":\"(?<hash>[^\"]*)\")");
    return CreateDictionaryFromLineRegex(regex, "hash", "desc", jsonDictionaryFile);
}

std::map<std::string, std::string> DictionaryFactory::CreateDictionaryFromLineRegex(const QRegularExpression& regex, const std::string& keyName, const std::string& valueName, const std::string& filePath) {
    std::map<std::string, std::string> dict;
    QFile file(QString::fromStdString(filePath));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                std::string key = match.captured(QString::fromStdString(keyName)).toStdString();
                if (dict.find(key) == dict.end()) {
                    dict[key] = match.captured(QString::fromStdString(valueName)).toStdString();
                }
            }
        }
    }
    return dict;
}

std::map<std::string, std::string> DictionaryFactory::CreateDictionaryFromCsv(const std::string& csvFile, int keyColumn, int valueColumn, bool skipFirstLine) {
    std::map<std::string, std::string> dict;
    int maxColIndex = std::max(keyColumn, valueColumn);
    int lineCount = 0;
    
    QFile file(QString::fromStdString(csvFile));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (lineCount > 0 || !skipFirstLine) {
                if (!line.isEmpty() && !line.startsWith("#")) {
                    QStringList cols = line.split(',');
                    if (cols.length() > maxColIndex) {
                        std::string key = cols[keyColumn].trimmed().toStdString();
                        if (dict.find(key) == dict.end()) {
                            dict[key] = cols[valueColumn].trimmed().toStdString();
                        }
                    }
                }
            }
            lineCount++;
        }
    }
    return dict;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
