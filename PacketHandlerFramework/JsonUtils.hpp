#pragma once

#include <vector>
#include <cstdint>
#include <map>
#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace PacketHandlerFramework {

class JsonUtils {
public:
    static std::map<std::string, std::vector<std::string>> GetParams(const std::vector<uint8_t>& data, bool gzip, int maxChildrenPerItem, bool& elementsHaveBeenSkipped);

private:
    static std::map<std::string, std::vector<std::string>> GetParams(const QJsonDocument& jsonDoc, int maxChildrenPerItem, bool& elementsHaveBeenSkipped);
    static void TraverseJson(const QJsonValue& val, const QString& parentKey, std::map<std::string, std::vector<std::string>>& elements, int maxChildrenPerItem, bool& elementsHaveBeenSkipped);
};

} // namespace PacketHandlerFramework